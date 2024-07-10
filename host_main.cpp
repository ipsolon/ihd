/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#include <iostream>

#include <boost/format.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

#include "ihd.h"
#include "safe_main.hpp"

#define NUMBER_OF_CHANNELS 1 /* We are limited to a single channel right now */

int IHD_SAFE_MAIN(int argc, char *argv[])
{
    std::cout << "Revision: " << ihd::get_version_string() << std::endl;
    std::string file, args;
    size_t total_num_samps, channel, spb;
    double freq, total_time;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "help message")
        ("file", po::value<std::string>(&file)->default_value("isrp_samples.dat"), "name of the file to write binary samples to")
        ("duration", po::value<double>(&total_time)->default_value(0), "total number of seconds to receive")
        ("nsamps", po::value<size_t>(&total_num_samps)->default_value(0), "total number of samples to receive")
        ("spb", po::value<size_t>(&spb)->default_value(4096), "samples per buffer")
        ("freq", po::value<double>(&freq)->default_value(0.0), "RF center frequency in Hz")
        ("channel", po::value<size_t>(&channel)->default_value(0), "which channel to use")
        ("args", po::value<std::string>(&args)->default_value(""), "ISRP device address args")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    //print the help message
    if (vm.count("help")) {
        std::cout << boost::format("IHD RX samples to file %s") % desc << std::endl;
        std::cout
            << std::endl
            << "This application streams data from a single channel of a Ipsolon device to a file.\n"
            << std::endl;
        return ~0;
    }

    /************************************************************************
     * Create device and block controls
     ***********************************************************************/
    std::cout << std::endl;
    std::cout << boost::format("Creating the ISRP device with: %s...") % args << std::endl;

    ihd::ipsolon_isrp::sptr isrp = ihd::ipsolon_isrp::make(args);
    uhd::tune_request_t tune_request{};
    tune_request.rf_freq = 5123456789;
    size_t chan = 0;
    isrp->set_rx_freq(tune_request, chan);

    uhd::stream_args_t stream_args("sc16", "sc16");
    std::vector<size_t> channel_nums;
    channel_nums.push_back(channel);
    stream_args.channels = channel_nums;
    auto rx_stream = isrp->get_rx_stream(stream_args);

    uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
    rx_stream->issue_stream_cmd(stream_cmd);
    std::vector<uint8_t *> buffs(NUMBER_OF_CHANNELS);
    try {
        buffs[0] = new uint8_t[spb];
    } catch (std::bad_alloc &exc) {
        UHD_LOGGER_ERROR("UHD")
                << "Bad memory allocation. "
                   "Try a smaller samples per buffer setting or free up additional memory";
        std::exit(EXIT_FAILURE);
    }
    uhd::rx_metadata_t md;
    for (int i = 0; i < 900; i++) {
        rx_stream->recv(buffs, spb, md, 5);
    }
    stream_cmd.stream_mode = uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS;
    rx_stream->issue_stream_cmd(stream_cmd);

    return 0;
}
