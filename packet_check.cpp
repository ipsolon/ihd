/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

#include <iostream>
#include <boost/format.hpp>
#include <boost/program_options.hpp>

#include "safe_main.hpp"
#include "ihd.h"

namespace po = boost::program_options;

#define NUMBER_OF_CHANNELS          1 /* We are limited to a single channel right now */

int IHD_SAFE_MAIN(int argc, char *argv[])
{
    std::cout << "Revision: " << ihd::get_version_string() << std::endl;
    std::string args;
    size_t channel, spb;
    uint32_t total_time;
    uhd::rx_metadata_t md;

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "help message")
            ("duration", po::value<uint32_t>(&total_time)->default_value(10), "total number of seconds to receive")
            ("spb", po::value<size_t>(&spb)->default_value(ihd::ipsolon_rx_stream::SAMPLES_PER_PACKET), "samples per buffer")
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
     * Allocate buffers
     ***********************************************************************/
    std::vector<std::complex<int16_t>*> buffs(NUMBER_OF_CHANNELS);
    std::complex<int16_t> p[spb];
    buffs[0] = p;

    /************************************************************************
     * Create device and block controls
     ***********************************************************************/
    std::cout << std::endl;
    std::cout << boost::format("Creating the ISRP device with: %s...") % args << std::endl;

    ihd::ipsolon_isrp::sptr isrp = ihd::ipsolon_isrp::make(args);

    /************************************************************************
     * Get Rx Stream
     ***********************************************************************/
    uhd::stream_args_t stream_args("sc16", "sc16");
    std::vector<size_t> channel_nums;
    channel_nums.push_back(channel);
    stream_args.channels = channel_nums;

    stream_args.args[ihd::ipsolon_rx_stream::stream_type::STREAM_FORMAT_KEY] =
            ihd::ipsolon_rx_stream::stream_type::FFT_STREAM;
    stream_args.args[ihd::ipsolon_rx_stream::stream_type::STREAM_DEST_IP_KEY] = "10.75.42.37";
    stream_args.args[ihd::ipsolon_rx_stream::stream_type::STREAM_DEST_PORT_KEY] = "9090";
    stream_args.args[ihd::ipsolon_rx_stream::stream_type::FFT_SIZE_KEY] = "256";
    stream_args.args[ihd::ipsolon_rx_stream::stream_type::FFT_AVG_COUNT_KEY] = "105";
    auto rx_stream = isrp->get_rx_stream(stream_args);

    /************************************************************************
     * Start the stream
     ***********************************************************************/
    uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
    rx_stream->issue_stream_cmd(stream_cmd);

    /************************************************************************
     * Receive Data
     ***********************************************************************/
    auto startTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::seconds(total_time);

    uint64_t errors = 0;
    uint64_t packets = 0;
    uint64_t bytes = 0;
    while (std::chrono::high_resolution_clock::now() - startTime < duration) {
        size_t n = rx_stream->recv(buffs, spb, md, 5);
        if (md.out_of_sequence) {
            fprintf(stderr, "*** OUT OF SEQUENCE PACKET:\n%s\n***\n", md.to_pp_string(false).c_str());
            errors++;
        }
        if (!n) {
            fprintf(stderr, "*** No bytes received:\n%s\n***\n", md.to_pp_string(false).c_str());
            errors++;
        }
        bytes += n;
        packets++;
    }
    printf("packets:%lu bytes:%lu errors:%lu\n", packets, bytes, errors);
    stream_cmd.stream_mode = uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS;
    rx_stream->issue_stream_cmd(stream_cmd);

    return(0);
}
