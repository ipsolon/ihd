//
// Created by jmeyers on 10/10/24.
//
#include <iostream>
#include <cstdint>
#include <arpa/inet.h>

#include <uhd/types/metadata.hpp>
#include <uhd/types/time_spec.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/utils/thread.hpp>

#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>


#include "ihd.h"
#include "chameleon_jammer_block_ctrl.hpp"

// Convenient namespacing for options parsing
namespace po = boost::program_options;
namespace pt = boost::property_tree;


void print_help_message() {
    printf("Help Message\n");
}

void get_engage_command__(int drone, ihd::jammer_config_t &command, int rescale, std::string dronesfile, float pgain) {
  pt::ptree drone_tree;
  pt::read_json(dronesfile, drone_tree);
  std::map<uint32_t, std::complex<float>> init;
  for ( pt::ptree::value_type &drone_def : drone_tree.get_child("drones") ) {
    //lout << dt() << "Reading drone" << std::endl;
    //lout << dt() << "dclass: " << drone_def.second.get<int>("dclass",0) << std::endl;
    //int dclass = drone_def.second.get<int>("dclass", 0);
    std::vector<int> dclasses;
    for ( pt::ptree::value_type dclass : drone_def.second.get_child("dclass") ) {
      dclasses.push_back(dclass.second.get_value<int>());
    }
    //if ( dclass == drone ) {
    int di = 0;
    for ( di = 0; di < dclasses.size(); di++) {
      if ( dclasses[di] == drone ) {
        break;
      }
    }
    if ( di != dclasses.size() ) {
      std::string dname = drone_def.second.get<std::string>("name", "unknown");
      std::string ddate = drone_def.second.get<std::string>("date", "unknown");

      //lout << clr(YELLOW) << dt() << "Engage pattern " << drone << ": " << dname << " " << ddate << cte << std::endl;
      printf("Engage pattern %d : %s", drone, dname.c_str());
      //frequency = drone_def.second.get<float>("frequency", 2.45e9);
 /*     if ( frequency == 0 ) {
        std::vector<double> frequencies;
        for ( pt::ptree::value_type freq : drone_def.second.get_child("frequency") ) {
          frequencies.push_back(freq.second.get_value<float>());
        }
        frequency = frequencies[di];
      }

      gain_override = drone_def.second.get<float>("gain_override",-1.0);*/
      //lout << dt() << "Frequency = " <<  frequency << std::endl;

      command.fm_max_dev = drone_def.second.get<float>("fm_max_dev",0.0);
      command.fm_ddang = drone_def.second.get<float>("fm_ddang", 0.0) * 4.0;
      command.dwell = drone_def.second.get<float>("dwell", 1.0);
      
      //lout << dt() << "fm_max_dev = " << command.fm_max_dev << std::endl;
      //lout << dt() << "fm_ddang = " << command.fm_ddang << std::endl;
      //lout << dt() << "dwell = " << command.dwell << std::endl;

      command.centers.clear();
      //lout << dt() << "centers: ";
      float pcenter;
      for ( pt::ptree::value_type ctr : drone_def.second.get_child("centers") ) {
          if ( rescale == 1 ) {
              pcenter = ctr.second.get_value<float>();
              pcenter = pcenter * 200/245.76;
              //command.centers.push_back(ctr.second.get_value<float>());
              printf("Using center: %f", pcenter);
              command.centers.push_back(pcenter);
          } else {
              command.centers.push_back(ctr.second.get_value<float>());
          }
        //lout << ctr.second.get_value<float>() << ",";
      }
      //lout << std::endl;

      std::vector<int> indices;
      int psindex;
      for ( pt::ptree::value_type index : drone_def.second.get_child("indices") ) {
          if ( rescale == 1 ) {
              psindex = index.second.get_value<int>();
              if ( psindex < 2048) {
                  psindex = psindex * 200/245.76;
              } else {
                  psindex = 4096 - ( 4096 - psindex ) * 200/245.76;
              }
              indices.push_back(psindex);
          } else {
              indices.push_back(index.second.get_value<int>());
          }
      }

      std::vector<double> reals;
      for ( pt::ptree::value_type real_part : drone_def.second.get_child("real_parts") ) {
        if ( false ) { //if(gnsspower < 0.2){
          //reals.push_back(real_part.second.get_value<float>()*(gnsspower*4.5+0.1));
        }
        else{
          reals.push_back(pgain * real_part.second.get_value<float>());
        }
      }

      std::vector<double> imags;
      for ( pt::ptree::value_type imag_part : drone_def.second.get_child("imag_parts") ) {
        if ( false ) { //if(gnsspower <0.2){
          //imags.push_back(imag_part.second.get_value<float>()*(gnsspower*4.5+0.1));
        }
        else{
          imags.push_back(pgain * imag_part.second.get_value<float>());
        }
      }

      for ( int i=0; i < indices.size(); i++ ) {
        init[indices[i]] = std::complex<float>(reals[i],imags[i]);
        //lout << dt() << "init[" << indices[i] << "] = (" << reals[i] << "," << imags[i] << ")" << std::endl;
      }
      command.phasors = init;

    }
  }
}


int UHD_SAFE_MAIN(int argc, char *argv[]) {
    uhd::set_thread_priority_safe();
    // Options
    double freq;
    double freq2;
    double freq3;
    float gain;
    float gain2;
    float gain3;
    std::string isrp_args;
    size_t channel;
    int drone;
    int drone2;
    int drone3;
    int rescale;
    std::string config;
    float pgain;
    int dozero;
    int duration;
    int exit_code = EXIT_SUCCESS;

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "Help message")
            ("config", po::value<std::string>(&config)->default_value("drones.json"),"Drone configuration file, default=drones.json")
            ("drone", po::value<int>(&drone)->default_value(6),"Drone code, ie. 6 - ocusync 2.4")
            ("drone2", po::value<int>(&drone2)->default_value(-1),"Drone code, ie. 6 - ocusync 2.4 for second channel")
            ("drone3", po::value<int>(&drone3)->default_value(-1),"Drone code, ie. 6 - ocusync 2.4 for third channel")
            ("rescale", po::value<int>(&rescale)->default_value(0),"Rescale waveforms for 245.76MHz samplerate, default=0")
            ("pgain", po::value<float>(&pgain)->default_value(1),"Adjust phasors for software gain, default=1 (no gain)")
            ("freq", po::value<double>(&freq)->default_value(2.45e9), "RF Center Frequency")
            ("freq2", po::value<double>(&freq2)->default_value(2.45e9), "RF Center Frequency on second channel")
            ("freq3", po::value<double>(&freq3)->default_value(2.45e9), "RF Center Frequency on third channel")
            ("gain", po::value<float>(&gain)->default_value(10.0), "TX Gain")
            ("gain2", po::value<float>(&gain2)->default_value(10.0), "TX Gain on second channel")
            ("gain3", po::value<float>(&gain3)->default_value(10.0), "TX Gain on second channel")
            ("channel", po::value<size_t>(&channel)->default_value(1), "which channel to use")
            ("zeroize", po::value<int>(&dozero)->default_value(0), "Zeroize jammer banks after tx, default=0")
            ("duration", po::value<int>(&duration)->default_value(20), "Transmit duration in seconds")
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
    ihd::ipsolon_isrp::temperature_t temps;

    int err = isrp->get_temperatures(temps, 1000);
    if (!err) {
        for (const auto &pair: temps) {
            std::cout << "Component Temp: " << pair.first << "=" << pair.second << std::endl;
        }
    } else {
        std::cout << "Unable to get temperatures" << std::endl;
    }

    auto ctrl_jammer = isrp->get_block_ctrl<ihd::chameleon_jammer_block_ctrl>(ihd::block_id_t(0));
    ihd::block_id_t blockid_jammer = ctrl_jammer->get_block_id();

    auto ctrl_jammer2 = isrp->get_block_ctrl<ihd::chameleon_jammer_block_ctrl>(ihd::block_id_t(1));
    ihd::block_id_t blockid_jammer2 = ctrl_jammer2->get_block_id();

    auto ctrl_jammer3 = isrp->get_block_ctrl<ihd::chameleon_jammer_block_ctrl>(ihd::block_id_t(2));
    ihd::block_id_t blockid_jammer3 = ctrl_jammer3->get_block_id();




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
    tune_request.args["qec_cal"] = "false";
    isrp->set_tx_freq(tune_request, channel);
    isrp->uhd::usrp::multi_usrp::set_tx_gain(gain, channel);
    printf("freq=%f, gain=%f , channel=%zu\n",freq,gain, channel);

    if ( drone2 != -1 ) {
        uhd::tune_request_t tune_request2{};
        tune_request2.rf_freq = freq2;
        tune_request2.args["calmask"] = "0x2200";
        isrp->set_tx_freq(tune_request2, 2);
        isrp->uhd::usrp::multi_usrp::set_tx_gain(gain2, 2);
        printf("freq=%f, gain=%f , channel=%d\n",freq2,gain2, 2);
    }

    if ( drone3 != -1 ) {
        uhd::tune_request_t tune_request3{};
        tune_request3.rf_freq = freq3;
        tune_request3.args["qec_cal"] = "false";
        isrp->set_tx_freq(tune_request3, 3);
        isrp->uhd::usrp::multi_usrp::set_tx_gain(gain3, 3);
        printf("freq=%f, gain=%f , channel=%d\n",freq3,gain3, 3);
    }

    //Todo
    // 1. read back tx_gain set
    // 2. read back freq set


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

    // Stream
    uhd::stream_args_t stream_args2("u32", "u32");
    uhd::device_addr_t target2;
    target2["block_id"] = blockid_jammer2.to_string();
    stream_args2.args = target2;

    std::vector<size_t> channel_nums2;
    channel_nums2.push_back(2);
    stream_args2.channels = channel_nums2;
    auto tx_stream2 = isrp->get_tx_stream(stream_args2);

    uhd::stream_args_t stream_args3("u32", "u32");
    uhd::device_addr_t target3;
    target3["block_id"] = blockid_jammer3.to_string();
    stream_args3.args = target3;

    std::vector<size_t> channel_nums3;
    channel_nums3.push_back(3);
    stream_args3.channels = channel_nums3;
    auto tx_stream3 = isrp->get_tx_stream(stream_args3);



    // Pass the TX stream over to the jammer
    ctrl_jammer->set_streamer(tx_stream);
    ctrl_jammer2->set_streamer(tx_stream2);
    ctrl_jammer3->set_streamer(tx_stream3);


    ihd::jammer_config_t zeroize;
    ihd::jammer_config_t a;
    ihd::jammer_config_t b;

    zeroize.bank = ihd::BANK_A;
    zeroize.dwell = 1;
    zeroize.fm_max_dev = 0.0f;
    zeroize.fm_ddang = 0.0f;
    zeroize.phasors = init;
    zeroize.centers = centers;

     if ( dozero == 1) {
        printf("Zeroizing Jammer Banks");
        zeroize.bank = ihd::BANK_A;
        ctrl_jammer->send_config(zeroize);
        if ( drone2 != -1 ) {
            ctrl_jammer2->send_config(zeroize);
        }
        if ( drone3 != -1 ) {
            ctrl_jammer3->send_config(zeroize);
        }
        zeroize.bank = ihd::BANK_B;
        ctrl_jammer->send_config(zeroize);
        if ( drone2 != -1 ) {
            ctrl_jammer2->send_config(zeroize);
        }
        if ( drone3 != -1 ) {
            ctrl_jammer3->send_config(zeroize);
        }

    }


    // Initialize A and B
   
    /*
    ctrl_jammer->send_config(zeroize);
    ctrl_jammer2->send_config(zeroize);
    zeroize.bank = ihd::BANK_B;
    ctrl_jammer->send_config(zeroize);
    ctrl_jammer2->send_config(zeroize);*/

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
    b.bank = ihd::BANK_B;
    /*
    a.dwell = 1;
    a.fm_ddang = 1.640625e-06;
    a.fm_max_dev = 1.953125e-03;
    a.phasors.clear();
    for (int i = 0; i < indices.size(); i++) {
        a.phasors[indices[i]] = std::complex<float>(real_parts[i], imag_parts[i]);
    }
    a.centers = {
            -8.589114e-01, 0.000000e+00, 8.589114e-01
    };*/
    get_engage_command__(drone,a, rescale, config, pgain);
    ctrl_jammer->send_config(a);
    a.bank = ihd::BANK_B;
    ctrl_jammer->send_config(a);

    if ( drone2 != -1 ) {
        get_engage_command__(drone2,b,rescale,config,pgain);
        b.bank = ihd::BANK_A;
        ctrl_jammer2->send_config(b);
        b.bank = ihd::BANK_B;
        ctrl_jammer2->send_config(b);
    }

    if ( drone3 != -1 ) {
        get_engage_command__(drone3,b,rescale,config,pgain);
        b.bank = ihd::BANK_A;
        ctrl_jammer3->send_config(b);
        b.bank = ihd::BANK_B;
        ctrl_jammer3->send_config(b);
    }

    /*
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
    */

    // Write configurations

    // Iterate back and forth
    auto radio_time = isrp->get_time_now();
    auto time = radio_time + uhd::time_spec_t(.10);

    for (uint32_t i = 0; i < duration; i++) {
        printf("Jamming from bank A\n");
        if (ctrl_jammer->start(ihd::BANK_A, time)) {
            exit_code = EXIT_FAILURE;
        }
        if ( drone2 != -1 ) {
            if (ctrl_jammer2->start(ihd::BANK_B, time)) {
                exit_code = EXIT_FAILURE;
            }
        }

        if ( drone3 != -1 ) {
            if (ctrl_jammer3->start(ihd::BANK_B, time)) {
                exit_code = EXIT_FAILURE;
            }
        }

        //time += uhd::time_spec_t(4096.0 * a.dwell / 245.76e6 + 0.1e-6); // 20 usecs
        usleep(1000000);
        /*printf("Jamming from bank B\n");
        ctrl_jammer->start(ihd::BANK_B, time);
        usleep(2000000);
        time += uhd::time_spec_t(4096.0 * b.dwell / 200.0e6 + 0.1e-6); // 204 usecs*/
    }

   
    ctrl_jammer->stop();
    if ( drone2 != -1 ) {
        ctrl_jammer2->stop();
    }
    if ( drone3 != -1 ) {
        ctrl_jammer3->stop();
    }

    auto total_time = (time - isrp->get_time_now()).get_full_secs();
    uint32_t sleeptime = static_cast<uint32_t>(total_time) + 2;
    printf("Sleeping for %d seconds\n", sleeptime);
    printf("Jammer overflow: %d\n", ctrl_jammer->overflow());
    ctrl_jammer->clear_overflow();
    if ( drone2 != -1 ) {
        ctrl_jammer2->clear_overflow();
    }
    if ( drone3 != -1 ) {
        ctrl_jammer3->clear_overflow();
    }

    if (exit_code == EXIT_FAILURE) {
        printf ("***** EXIT_ERROR!!! ");
    }
    return exit_code;
}
