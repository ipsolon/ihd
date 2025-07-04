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
#include "debug.hpp"

namespace po = boost::program_options;

#define NUMBER_OF_CHANNELS          1 /* We are limited to a single channel right now */
#define DEFAULT_FREQ       2400000000.0
#define DEFAULT_FFT_AVG    120
#define DEFAULT_FFT_SIZE   256

int IHD_SAFE_MAIN(int argc, char *argv[]) {
    std::cout << "Revision: " << ihd::get_version_string() << std::endl;
    std::string file, args;
    size_t total_num_samps, channel;
    double freq, total_time, gain;
    uhd::rx_metadata_t md;

    std::string dest_ip;
    uint16_t dest_port;
    uint32_t fft_size;
    uint32_t fft_avg;
    uint32_t packet_size;
    std::string stream_type;

    bool do_qec_cal;

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "help message")
            ("file", po::value<std::string>(&file)->default_value("isrp_samples.dat"),
             "name of the file to write binary samples to")
            ("duration", po::value<double>(&total_time)->default_value(30), "total number of seconds to receive")
            ("gain", po::value<double>(&gain)->default_value(0), "set RX gain")
            ("nsamps", po::value<size_t>(&total_num_samps)->default_value(0), "total number of samples to receive.")
            ("freq", po::value<double>(&freq)->default_value(DEFAULT_FREQ), "RF center frequency in Hz")
            ("channel", po::value<size_t>(&channel)->default_value(1), "which channel to use")
            ("dest_ip", po::value<std::string>(&dest_ip)->default_value("0.0.0.0"), "Destination IP address")
            ("dest_port", po::value<uint16_t>(&dest_port)->default_value(9090), "Destination port")
            ("fft_size", po::value<uint32_t>(&fft_size)->default_value(DEFAULT_FFT_SIZE),
             "FFT size (256, 512, 1024, 2048 or 4096")
            ("fft_avg", po::value<uint32_t>(&fft_avg)->default_value(DEFAULT_FFT_AVG), "FFT averaging count")
            ("args", po::value<std::string>(&args)->default_value(""), "ISRP device address args")
            ("stream_type", po::value<std::string>(&stream_type)->default_value("psd"), "Stream type - (psd or iq)")
            ("qec_cal", po::value<bool>(&do_qec_cal)->default_value(true), "Do QEC calibration when changing frequency");
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

    if (vm["args"].defaulted()) {
        std::cout
                << std::endl
                << "ERROR 'args' is a mandatory parameter please specify the chameleon ip address, e.g. args=addr=10.75.42.209 "
                << std::endl
                << boost::format("IHD RX samples to file %s") % desc << std::endl;
    }

    int fd = open(file.c_str(), O_CREAT | O_TRUNC | O_RDWR,
                  S_IRUSR | S_IWUSR | S_IRGRP |
                  S_IWGRP | S_IROTH | S_IWOTH);
    if (fd < 0) {
        perror("File Open error");
        return -1;
    }

    /************************************************************************
     * Create device and block controls
     ***********************************************************************/
    std::cout << std::endl;
    std::cout << boost::format("Creating the ISRP device with: %s...") % args << std::endl;

    ihd::ipsolon_isrp::sptr isrp = nullptr;
    try {
        isrp = ihd::ipsolon_isrp::make(args);

        isrp->stream_stop_all();

        // set the frequency
        uhd::tune_request_t tune_request{};
        tune_request.rf_freq = freq;
        tune_request.args["qec_cal"] = do_qec_cal ? "true":"false";
        isrp->set_rx_freq(tune_request, channel);

        // set the gain
        isrp->uhd::usrp::multi_usrp::set_rx_gain(gain, channel);

        /************************************************************************
         * Get Rx Stream
         ***********************************************************************/
        uhd::stream_args_t stream_args("sc16", "sc16");
        std::vector<size_t> channel_nums;
        channel_nums.push_back(channel);
        stream_args.channels = channel_nums;

        if (stream_type == ihd::ipsolon_rx_stream::stream_type::PSD_STREAM) {
            stream_args.args[ihd::ipsolon_rx_stream::stream_type::STREAM_FORMAT_KEY] =
                    ihd::ipsolon_rx_stream::stream_type::PSD_STREAM;
            stream_args.args[ihd::ipsolon_rx_stream::stream_type::STREAM_DEST_IP_KEY] =
                    dest_ip;
            stream_args.args[ihd::ipsolon_rx_stream::stream_type::STREAM_DEST_PORT_KEY] =
                    std::to_string(dest_port);
            stream_args.args[ihd::ipsolon_rx_stream::stream_type::FFT_SIZE_KEY] =
                    std::to_string(fft_size);
            stream_args.args[ihd::ipsolon_rx_stream::stream_type::FFT_AVG_COUNT_KEY] =
                    std::to_string(fft_avg);
        } else if (stream_type == ihd::ipsolon_rx_stream::stream_type::IQ_STREAM) {
            stream_args.args[ihd::ipsolon_rx_stream::stream_type::STREAM_FORMAT_KEY] =
                    ihd::ipsolon_rx_stream::stream_type::IQ_STREAM;
            stream_args.args[ihd::ipsolon_rx_stream::stream_type::STREAM_DEST_IP_KEY] =
                    dest_ip;
            stream_args.args[ihd::ipsolon_rx_stream::stream_type::STREAM_DEST_PORT_KEY] =
                    std::to_string(dest_port);
        } else {
            fprintf(stderr, "Error: Invalid stream_type\n");
            exit(1);
        }

        auto rx_stream = isrp->get_rx_stream(stream_args);
        if (vm["nsamps"].defaulted()) {
            if (stream_type == ihd::ipsolon_rx_stream::stream_type::IQ_STREAM) {
                // FIXME Temporary set total_num_samps to current max limit
                total_num_samps = rx_stream->get_max_num_samps() * TEMP_PACKET_LIMIT;
            } else {
                total_num_samps = rx_stream->get_max_num_samps() * 100;
            }
        }

        /************************************************************************
         * Start the stream
         ***********************************************************************/
        uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
        rx_stream->issue_stream_cmd(stream_cmd);

        /************************************************************************
        * Allocate buffers
        ***********************************************************************/
        std::vector<std::complex<int16_t> *> buffs(NUMBER_OF_CHANNELS);
        size_t spb = rx_stream->get_max_num_samps();
        std::complex<int16_t> p[spb];
        buffs[0] = p;

        /************************************************************************
         * Receive Data
         ***********************************************************************/
        dbprintf("total_num_samps = %lu spb=%lu\n",total_num_samps,spb);
        size_t sample_iterations = total_num_samps / spb;
        dbprintf("sample_iterations = %lu \n",sample_iterations);
        int err = 0;
        size_t out_of_sequence_packets = 0;
        dbprintf("Receiving data sample_iterations = %lu\n", sample_iterations);
        for (size_t i = 0; i < sample_iterations && !err; i++) {
            size_t n = rx_stream->recv(buffs, spb, md, 5);
            if (md.out_of_sequence) {
                out_of_sequence_packets++;
            }
            if (!n) {
                fprintf(stderr, "*** No bytes received:\n%s\n***\n", md.to_pp_string(false).c_str());
                err = -1;
            }
            size_t ws = n * ihd::ipsolon_rx_stream::BYTES_PER_IQ_PAIR;
            ssize_t w = write(fd, buffs[0], ws);
            if (w != ws) {
                fprintf(stderr, "Write failed. Request %lu bytes written, write returned:%lu. %s\n",
                        n, w, strerror(errno));
            }
        }
        stream_cmd.stream_mode = uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS;
        rx_stream->issue_stream_cmd(stream_cmd);
        close(fd);

        if (out_of_sequence_packets) {
            fprintf(stderr, "*** Out of sequence packets:%zu\n", out_of_sequence_packets);
            return -1;
        }
    } catch (const std::exception &exc) {
        printf("EXCEPTION: %s", exc.what());
        exit(-1);
    }
    return 0;
}
