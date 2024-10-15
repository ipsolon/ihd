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
#define DEFAULT_FREQ       2400000000.0

int IHD_SAFE_MAIN(int argc, char *argv[])
{
    std::cout << "Revision: " << ihd::get_version_string() << std::endl;
    std::string file, args;
    size_t total_num_samps, channel, spb;
    double freq, total_time;
    uhd::rx_metadata_t md;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "help message")
        ("file", po::value<std::string>(&file)->default_value("isrp_samples.dat"), "name of the file to write binary samples to")
        ("duration", po::value<double>(&total_time)->default_value(0), "total number of seconds to receive")
        ("nsamps", po::value<size_t>(&total_num_samps)->default_value(ihd::ipsolon_rx_stream::SAMPLES_PER_PACKET * 10), "total number of samples to receive")
        ("spb", po::value<size_t>(&spb)->default_value(ihd::ipsolon_rx_stream::SAMPLES_PER_PACKET), "samples per buffer")
        ("freq", po::value<double>(&freq)->default_value(DEFAULT_FREQ), "RF center frequency in Hz")
        ("channel", po::value<size_t>(&channel)->default_value(0), "which channel to use")
        ("args", po::value<std::string>(&args)->default_value(""), "ISRP device address args")
        ("fft","Stream FFTs (versus an I/Q stream)")
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

    int fd = open(file.c_str(), O_CREAT | O_TRUNC | O_RDWR,
                                        S_IRUSR | S_IWUSR | S_IRGRP |
                                        S_IWGRP | S_IROTH | S_IWOTH);
    if (fd < 0) {
        perror("File Open error");
        exit(fd);
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

    if (!vm["freq"].defaulted()) {
        uhd::tune_request_t tune_request{};
        tune_request.rf_freq = freq;
        size_t chan = 0;
        isrp->set_rx_freq(tune_request, chan);
    }
    /************************************************************************
     * Get Rx Stream
     ***********************************************************************/
    // FIXME - You should NOT have to just 'know' the data type here
    uhd::stream_args_t stream_args("sc16", "sc16");
    std::vector<size_t> channel_nums;
    channel_nums.push_back(channel);
    stream_args.channels = channel_nums;

    // TODO - There is currently no difference right now.  In the future you will be able to
    //           to select what you want in you stream, raw IQ, FFT, etc.
    if (vm.count("fft")) {
        stream_args.args[ihd::ipsolon_rx_stream::stream_type::STREAM_FORMAT_KEY] =
                         ihd::ipsolon_rx_stream::stream_type::FFT_STREAM;
    } else {
        stream_args.args[ihd::ipsolon_rx_stream::stream_type::STREAM_FORMAT_KEY] =
                ihd::ipsolon_rx_stream::stream_type::IQ_STREAM;
    }
    auto rx_stream = isrp->get_rx_stream(stream_args);

    /************************************************************************
     * Start the stream
     ***********************************************************************/
    uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
    rx_stream->issue_stream_cmd(stream_cmd);

    /************************************************************************
     * Receive Data
     ***********************************************************************/
    size_t sample_iterations = total_num_samps / spb;
    int err = 0;
    for (size_t i = 0; i < sample_iterations && !err; i++) {
        size_t n = rx_stream->recv(buffs, spb, md, 5);
        if (md.out_of_sequence) {
            fprintf(stderr, "*** OUT OF SEQUENCE PACKET:\n%s\n***\n", md.to_pp_string(false).c_str());
            err = -1;
        }
        if (!n) {
            fprintf(stderr, "*** No bytes received:\n%s\n***\n", md.to_pp_string(false).c_str());
            err = -1;
        }
        size_t ws = n *  ihd::ipsolon_rx_stream::BYTES_PER_IQ_PAIR;
        ssize_t w = write(fd, buffs[0], ws);
        if (w != ws) {
            fprintf(stderr, "Write failed. Request %lu bytes written, write returned:%lu. %s\n",
                    n, w, strerror(errno));
        }
    }
    stream_cmd.stream_mode = uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS;
    rx_stream->issue_stream_cmd(stream_cmd);

    close(fd);
    return(0);
}
