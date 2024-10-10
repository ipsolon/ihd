//
// Created by jmeyers on 10/10/24.
//

#include <iostream>
#include <csignal>
#include <cstdint>

#include <boost/program_options.hpp>
#include <uhd/types/metadata.hpp>
#include <uhd/types/time_spec.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/utils/thread.hpp>
#include <uhd/rfnoc/radio_ctrl.hpp>

#include <citadel/rfnoc/timed_jammer_block_ctrl.hpp>

#include <uhd/convert.hpp>
#include <arpa/inet.h>

uhd::convert::converter::~converter() { }

class convert_u32_u32be : public uhd::convert::converter {
public:
    convert_u32_u32be(void) { }
    void set_scalar(const double scalar) { (void)scalar ; }
private:
    void operator()(const input_type &inputs, const output_type &outputs, const size_t nsamps) {
        const uint32_t *input = reinterpret_cast<const uint32_t *>(inputs[0]) ;
        uint32_t *output = reinterpret_cast<uint32_t *>(outputs[0]) ;

        size_t i ;
        for(i = 0 ; i < nsamps ; i++ ) {
            output[i] = htonl(input[i]) ;
        }
    }
} ;

static uhd::convert::converter::sptr make_convert_u32_u32be(void) {
    return uhd::convert::converter::sptr(new convert_u32_u32be()) ;
}

void register_converter() {
    uhd::convert::register_bytes_per_item("u32", sizeof(uint32_t)) ;

    uhd::convert::id_type id ;
    id.num_inputs = 1 ;
    id.num_outputs = 1 ;
    id.input_format = "u32" ;
    id.output_format = "u32_item32_be" ;
    uhd::convert::register_converter(id, &make_convert_u32_u32be, 0) ;
}

// Convenient namespacing for options parsing
namespace po = boost::program_options ;

void print_help_message() {
    printf( "Help Message\n" ) ;
    return ;
}

int UHD_SAFE_MAIN(int argc, char *argv[]) {
    uhd::set_thread_priority_safe() ;

    // Options
    float freq ;
    float gain ;
    std::string fname ;
    uint32_t nsamps ;
    std::string usrp_args ;
    float time ;

    po::options_description desc("Allowed options") ;
    desc.add_options()
            ("help",                                                                                                                "Help message")
            ("freq",        po::value<float>(&freq)->default_value(2.45e9),                                                         "RF Center Frequency")
            ("gain",        po::value<float>(&gain)->default_value(10.0),                                                           "TX Gain")
            ("time",        po::value<float>(&time)->default_value(10.0),                                                           "Sleep time")
            ("file",        po::value<std::string>(&fname)->default_value(""),                                                      "File to save looped back samples from")
            ("n",           po::value<uint32_t>(&nsamps)->default_value(1000000),                                                   "Number of samples to read in file mode")
            ("args",        po::value<std::string>(&usrp_args)->default_value("addr=192.168.30.2,second_addr=192.168.40.2"),        "UHD Device Arguments")
            ;

    po::variables_map vm ;
    po::store(po::parse_command_line(argc, argv, desc), vm) ;
    po::notify(vm) ;
    if( vm.count("help") ) {
        print_help_message() ;
        return EXIT_SUCCESS ;
    }

    register_converter() ;

    // Create a new context to force deletion of objects below
    {
        printf( "Creating USRP with: %s\n", usrp_args.c_str() ) ;
        auto usrp = uhd::device3::make(usrp_args) ;

        // Control and block ID's for the RFNoC components
        auto ctrl_radio     = usrp->get_block_ctrl<uhd::rfnoc::radio_ctrl>(uhd::rfnoc::block_id_t(0, "Radio", 0)) ;
        auto ctrl_jammer    = usrp->get_block_ctrl<citadel::rfnoc::timed_jammer_block_ctrl>(uhd::rfnoc::block_id_t(0, "CitadelTimedJammer")) ;

        auto blockid_radio  = ctrl_radio->get_block_id() ;
        auto blockid_jammer = ctrl_jammer->get_block_id() ;

        // Initialize bank A
        std::map<uint32_t, std::complex<float>> init ;
        for(uint32_t idx = 0 ; idx < 4096 ; idx++) {
            init[idx] = std::complex<float>(0.0f, 0.0f) ;
        }

        std::vector<float> centers ;
        centers.push_back(0.0f) ;

        // Jammer -> Radio
        if( fname == "" ) {
            printf( "Streaming Jammer -> Radio\n" ) ;
            // Setting TX Frequency and Gain setting
            ctrl_radio->set_tx_frequency(freq, 0) ;
            ctrl_radio->set_tx_gain(gain, 0) ;
            ctrl_radio->set_arg<uint32_t>("spp", 4096) ;
            printf( "Actual frequency: %14.8f\n", ctrl_radio->get_tx_frequency(0) ) ;

            // Create and connect the graph
            auto tx_graph = usrp->create_graph("TX") ;
            tx_graph->connect( blockid_jammer, 0, blockid_radio, uhd::rfnoc::ANY_PORT) ;

            // Metadata
            uhd::tx_metadata_t md ;
            md.start_of_burst   = true ;
            md.end_of_burst     = true ;
            md.has_time_spec    = false ;

            // Stream
            uhd::stream_args_t stream_args("u32", "u32") ;
            uhd::device_addr_t target ;
            target["block_id"] = blockid_jammer ;
            stream_args.args = target ;
            stream_args.channels = std::vector<size_t>(1) ;
            auto tx_stream = usrp->get_tx_stream(stream_args) ;

            // Pass the TX stream over to the jammer
            ctrl_jammer->set_streamer(tx_stream) ;

            citadel::rfnoc::jammer_config_t zeroize ;

            citadel::rfnoc::jammer_config_t a ;
            citadel::rfnoc::jammer_config_t b ;

            zeroize.bank = citadel::rfnoc::BANK_A ;
            zeroize.dwell = 1 ;
            zeroize.fm_max_dev = 0.0f ;
            zeroize.fm_ddang = 0.0f ;
            zeroize.phasors = init ;
            zeroize.centers = centers ;

            // Initialize A and B
            ctrl_jammer->send_config(zeroize) ;
            zeroize.bank = citadel::rfnoc::BANK_B ;
            ctrl_jammer->send_config(zeroize) ;

            // A Configuration
            a.bank = citadel::rfnoc::BANK_A ;
            a.dwell = 1 ;
            a.fm_max_dev = 0.0138f ;
            a.fm_ddang = 0.0035f ;
            centers.clear();
            centers.push_back(-.7854) ;
            centers.push_back(-.0314);
            centers.push_back(.7854);
            a.centers = centers ;
            a.phasors.clear() ;
            init.clear();
            init[1802] = std::complex<float>(855.4326/855.0,0);
            init[1843] = std::complex<float>(8.3058e2/855.0,2.0472e2/855.0);
            init[1884] = std::complex<float>(4.8594e2/855.0,7.0401e2/855.0);
            init[1925] = std::complex<float>(-4.8594e2/855.0,7.0401e2/855.0);
            init[1966] = std::complex<float>(-6.4030e2/855.0,-5.6726e2/855.0);
            init[2007] = std::complex<float>(8.3058e2/855.0,-2.0472e2/855.0);
            init[2048] = std::complex<float>(-6.4030e2/855.0,5.6726e2/855.0);
            init[2089] = std::complex<float>(6.4030e2/855.0,-5.6726e2/855.0);
            init[2130] = std::complex<float>(-8.3058e2/855.0,2.0472e2/855.0);
            init[2171] = std::complex<float>(6.4030e2/855.0,5.6726e2/855.0);
            init[2212] = std::complex<float>(4.8594e2/855.0,-7.0401e2/855.0);
            init[2253] = std::complex<float>(-4.8594e2/855.0,-7.0401e2/855.0);
            init[2294] = std::complex<float>(-8.3058e2/855.0,-2.0472e2/855.0);

            a.phasors = init;

            // B Configuration
            b.bank = citadel::rfnoc::BANK_B ;
            b.dwell = 10 ;
            b.fm_max_dev = 0.0f ;
            b.fm_ddang = 0.0f ;
            b.centers = centers ;
            b.phasors.clear() ;
            b.phasors[0] = std::complex<float>(0.25f, 0.0f) ;
            b.phasors[409] = std::complex<float>(0.0f, 0.25f) ;
            b.phasors[612] = std::complex<float>(0.25f, 0.25f) ;

            // Write configurations
            ctrl_jammer->send_config(a) ;
            //a.bank = citadel::rfnoc::BANK_B;
            ctrl_jammer->send_config(b) ;

            // Iterate back and forth
            auto radio_time = ctrl_radio->get_time_now() ;
            auto time = radio_time + uhd::time_spec_t(1.0) ;

            for(uint32_t i = 0 ; i < 100000 ; i++) {
                printf("Jamming from bank A");
                ctrl_jammer->start(citadel::rfnoc::BANK_A, time) ;
                time += uhd::time_spec_t(4096.0*a.dwell/200.0e6 + 0.1e-6) ; // 20 usecs
                printf("Jamming from bank B");
                ctrl_jammer->start(citadel::rfnoc::BANK_B, time) ;
                time += uhd::time_spec_t(4096.0*b.dwell/200.0e6 + 0.1e-6) ; // 204 usecs
            }

            ctrl_jammer->stop() ;

            auto total_time = (time - ctrl_radio->get_time_now()).get_full_secs() ;
            uint32_t sleeptime = static_cast<uint32_t>(total_time) + 2 ;
            printf( "Sleeping for %d seconds\n", sleeptime ) ;
            printf( "Jammer overflow: %d\n", ctrl_jammer->overflow() ) ;
            ctrl_jammer->clear_overflow() ;

        } else {
            printf( "Streaming Jammer -> File\n" ) ;
            uhd::stream_args_t rx_args("fc32", "sc16") ;
            uhd::device_addr_t target ;
            target["block_id"] = blockid_jammer ;
            rx_args.args = target ;
            auto rx_stream = usrp->get_rx_stream(rx_args) ;
            std::vector<std::complex<float>> samples(nsamps) ;

            uhd::rx_metadata_t md ;
            auto rv = rx_stream->recv(&samples.front(), samples.size(), md, 1.0) ;
            (void)rv ;

            if( md.error_code != uhd::rx_metadata_t::error_code_t::ERROR_CODE_NONE ) {
                printf( "RX Error: %s\n", md.to_pp_string().c_str() ) ;
            }
            auto fout = fopen( fname.c_str(), "w" ) ;
            if( fout == NULL ) {
                printf( "Couldn't open %s\n", fname.c_str() ) ;
            } else {
                for( std::complex<float> sample : samples ) {
                    fprintf( fout, "%14.8f %14.8f\n", sample.real(), sample.imag() ) ;
                }
                fclose( fout ) ;
            }
        }
    }

    return EXIT_SUCCESS ;
}
