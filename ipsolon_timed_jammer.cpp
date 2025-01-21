//
// Created by jmeyers on 10/10/24.
//
#include <iostream>
#include <cstdint>
#include <arpa/inet.h>

#include <boost/program_options.hpp>
#include <uhd/types/metadata.hpp>
#include <uhd/types/time_spec.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/utils/thread.hpp>
#include <uhd/convert.hpp>

#include "ihd.h"
#include "chameleon_jammer_block_ctrl.hpp"

// Convenient namespacing for options parsing
namespace po = boost::program_options;

void print_help_message() {
    printf("Help Message\n");
}

int UHD_SAFE_MAIN(int argc, char *argv[]) {
    uhd::set_thread_priority_safe();
    // Options
    float freq;
    float gain;
    std::string isrp_args;
    size_t channel;

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "Help message")
            ("freq", po::value<float>(&freq)->default_value(2.45e9), "RF Center Frequency")
            ("gain", po::value<float>(&gain)->default_value(10.0), "TX Gain")
            ("channel", po::value<size_t>(&channel)->default_value(1), "which channel to use")
            ("args", po::value<std::string>(&isrp_args)->default_value("addr=192.168.0.100"),
                    "UHD Device Arguments");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
        print_help_message();
        return EXIT_SUCCESS;
    }

    printf("Creating USRP with: %s\n", isrp_args.c_str());
    auto isrp = ihd::ipsolon_isrp::make(isrp_args);

    auto ctrl_jammer = isrp->get_block_ctrl<ihd::chameleon_jammer_block_ctrl>(ihd::block_id_t(0));
    ihd::block_id_t blockid_jammer = ctrl_jammer->get_block_id();

    // Initialize both banks with zeros
    std::map<uint32_t, std::complex<float>> init;
    for (uint32_t idx = 0; idx < 4096; idx++) {
        init[idx] = std::complex<float>(0.0f, 0.0f);
    }
    std::vector<float> centers;
    centers.push_back(0.0f);

    // Setting TX Frequency and Gain setting
    uhd::tune_request_t tune_request{};
    tune_request.rf_freq = freq;
    isrp->set_tx_freq(tune_request, channel);
    double freq_ack=  isrp->get_tx_freq(channel);
    isrp->uhd::usrp::multi_usrp::set_tx_gain(gain, channel);
    double txgain_ack= isrp->uhd::usrp::multi_usrp::get_tx_gain(channel);;
    if (freq_ack <0 || txgain_ack < 0 )
    {
        std::cout<<"jammer configure problem: " << " channel="<< channel << " freq="<<freq_ack << " tx_gain ="<<txgain_ack <<"\n";
        exit(-1);
    }
    else
    {
        std::cout<<"jammer configure with: " << " channel="<< channel << " freq="<<freq_ack << " tx_gain ="<<txgain_ack <<"\n";
    }

    // Metadata
    uhd::tx_metadata_t md;
    md.start_of_burst = true;
    md.end_of_burst = true;
    md.has_time_spec = false;

    // Stream
    uhd::stream_args_t stream_args("u32", "u32");
    uhd::device_addr_t target;
    target["block_id"] = blockid_jammer.to_string();
    stream_args.args = target;

    std::vector<size_t> channel_nums;
    channel_nums.push_back(channel);
    stream_args.channels = channel_nums;
    auto tx_stream = isrp->get_tx_stream(stream_args);

    // Pass the TX stream over to the jammer
    ctrl_jammer->set_streamer(tx_stream);

    ihd::jammer_config_t zeroize;
    ihd::jammer_config_t a;
    ihd::jammer_config_t b;

    zeroize.bank = ihd::BANK_A;
    zeroize.dwell = 1;
    zeroize.fm_max_dev = 0.0f;
    zeroize.fm_ddang = 0.0f;
    zeroize.phasors = init;
    zeroize.centers = centers;

    // Initialize A and B
    ctrl_jammer->send_config(zeroize);
    zeroize.bank = ihd::BANK_B;
    ctrl_jammer->send_config(zeroize);

    // Bank A - Titan example
    std::vector<int>  indices = {
            3824, 3840, 3856, 3872, 3888,
            3904, 3920, 3936, 3952, 3968,
            3984, 4000, 4016, 4032, 4048,
            4064, 4080,    0,   16,   32,
              48,   64,   80,   96,  112,
             128,  144,  160,  176,  192,
             208,  224,  240,  256,  272
    };
    std::vector<float> real_parts = {
            1.243725e-01,  1.238718e-01,  1.164418e-01,  8.594916e-02, 1.669492e-02,
            -7.754496e-02, -1.238718e-01, -3.843320e-02,  1.067674e-01, 6.851641e-02,
            -1.120557e-01, -1.669492e-02,  1.164418e-01, -1.067674e-01, 3.843320e-02,
            2.767547e-02, -6.851641e-02,  8.594916e-02, -8.594916e-02, 6.851641e-02,
            -2.767547e-02, -3.843320e-02,  1.067674e-01, -1.164418e-01, 1.669492e-02,
            1.120557e-01, -6.851641e-02, -1.067674e-01,  3.843320e-02, 1.238718e-01,
            7.754496e-02, -1.669492e-02, -8.594916e-02, -1.164418e-01, -1.238718e-01
    };
    std::vector<float> imag_parts = {
            0.000000e+00,  1.114866e-02,  4.370135e-02,  8.989577e-02,  1.232469e-01,
            9.723830e-02, -1.114866e-02, -1.182852e-01, -6.379054e-02,  1.037979e-01,
            5.396319e-02, -1.232469e-01,  4.370135e-02,  6.379054e-02, -1.182852e-01,
            1.212542e-01, -1.037979e-01,  8.989577e-02, -8.989577e-02,  1.037979e-01,
            -1.212542e-01,  1.182852e-01, -6.379054e-02, -4.370135e-02,  1.232469e-01,
            -5.396319e-02, -1.037979e-01,  6.379054e-02,  1.182852e-01,  1.114866e-02,
            -9.723830e-02, -1.232469e-01, -8.989577e-02, -4.370135e-02, -1.114866e-02
    };
    a.bank = ihd::BANK_A;
    a.dwell = 1;
    a.fm_ddang = 1.640625e-06;
    a.fm_max_dev = 1.953125e-03;
    a.phasors.clear();
    for (int i = 0; i < indices.size(); i++) {
        a.phasors[indices[i]] = std::complex<float>(real_parts[i], imag_parts[i]);
    }
    a.centers = {
            -8.589114e-01, 0.000000e+00, 8.589114e-01
    };

    // Bank B - Citadel example
    b.bank = ihd::BANK_B;
    b.dwell = 1;
    b.fm_ddang = 1.640625e-06;
    b.fm_max_dev = 1.953125e-03;
    b.phasors.clear();
    for (int i = 0; i < indices.size(); i++) {
        b.phasors[indices[i]] = std::complex<float>(real_parts[i], imag_parts[i]);
    }
    b.centers = {-8.589114e-01};

    // Write configurations
    ctrl_jammer->send_config(a);
    ctrl_jammer->send_config(b);

    // Iterate back and forth
    auto radio_time = isrp->get_time_now();
    auto time = radio_time + uhd::time_spec_t(1.0);

    for (uint32_t i = 0; i < 10; i++) {
        printf("Jamming from bank A\n");
        ctrl_jammer->start(ihd::BANK_A, time);
        time += uhd::time_spec_t(4096.0 * a.dwell / 200.0e6 + 0.1e-6); // 20 usecs
        usleep(2000000);
        printf("Jamming from bank B\n");
        ctrl_jammer->start(ihd::BANK_B, time);
        usleep(2000000);
        time += uhd::time_spec_t(4096.0 * b.dwell / 200.0e6 + 0.1e-6); // 204 usecs
    }

    ctrl_jammer->stop();

    auto total_time = (time - isrp->get_time_now()).get_full_secs();
    uint32_t sleeptime = static_cast<uint32_t>(total_time) + 2;
    printf("Sleeping for %d seconds\n", sleeptime);
    printf("Jammer overflow: %d\n", ctrl_jammer->overflow());
    ctrl_jammer->clear_overflow();

    return EXIT_SUCCESS;
}
