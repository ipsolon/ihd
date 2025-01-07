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
#include <thread>

#include "safe_main.hpp"
#include "ihd.h"

namespace po = boost::program_options;

#define NUMBER_OF_CHANNELS          1 /* We are limited to a single channel per stream right now */

class RxStream {
public:
    RxStream(size_t chan, ihd::ipsolon_isrp::sptr &isrp, uint32_t tt,
             uhd::stream_args_t &stream_args) : channel(chan), total_time(tt) {
        std::vector<size_t> channel_nums;
        channel_nums.push_back(chan);
        stream_args.channels = channel_nums;
        rx_stream = isrp->get_rx_stream(stream_args);
    }

    // Start a threads to receive iq/psd data - one for each channel enabled in the chan_mask
    virtual void run_loop() = 0;

    void stream_run() {
        std::cout << "Run stream for channel:" << channel << std::endl;

        /************************************************************************
         * Start the stream
         ***********************************************************************/
        uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
        rx_stream->issue_stream_cmd(stream_cmd);

        /************************************************************************
         * Receive Data
         ***********************************************************************/
        run_loop();
        stream_cmd.stream_mode = uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS;
        rx_stream->issue_stream_cmd(stream_cmd);
    }

private:
    uhd::stream_args_t stream_args{};
    size_t channel{};

protected:
    uint32_t total_time{};
    uhd::rx_streamer::sptr rx_stream{};
};

class RxStreamPsd : public RxStream {
public:
    RxStreamPsd(size_t chan, ihd::ipsolon_isrp::sptr &isrp, uint32_t tt, uhd::stream_args_t &stream_args)
        : RxStream(chan, isrp, tt, stream_args) {
    }

    virtual ~RxStreamPsd() = default;

private:
    void run_loop() override {
        uhd::rx_metadata_t md;
        /************************************************************************
         * Allocate buffers
         ***********************************************************************/
        std::vector<std::complex<int16_t> *> buffs(NUMBER_OF_CHANNELS);
        size_t spb = rx_stream->get_max_num_samps();
        std::complex<int16_t> p[spb];
        buffs[0] = p;
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
            } else {
                packets++;
            }
            bytes += n;
        }
        auto finishTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - startTime);
        bytes *= 4; /* bytes = samples * 4 */
        bytes += 16 * packets; /* Account for CHDR */
        double megabits_per_second = (((double) bytes / total_time) / (1024 * 1024)) * 8;
        printf("RESULT duration ms:%ld packets:%lu bytes:%lu Mb/s:%f errors:%lu\n",
               finishTime.count(), packets, bytes, megabits_per_second, errors);
    }
};

class RxStreamIq : public RxStream {
public:
    RxStreamIq(size_t chan, ihd::ipsolon_isrp::sptr &isrp, uint32_t tt, uhd::stream_args_t &stream_args)
        : RxStream(chan, isrp, tt, stream_args) {
    }

    virtual ~RxStreamIq() = default;

private:
    void run_loop() override {
        uhd::rx_metadata_t md;
        /************************************************************************
         * Allocate buffers
         ***********************************************************************/
        std::vector<std::complex<int16_t> *> buffs(NUMBER_OF_CHANNELS);
        size_t spb = rx_stream->get_max_num_samps();
        std::complex<int16_t> p[spb];
        buffs[0] = p;
        auto startTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::seconds(total_time);

        uint64_t errors = 0;
        uint64_t packets = 0;
        uint64_t bytes = 0;
        // FIXME - TEMP_PACKET_LIMIT is temporary until udp keeps up with dma
        while ((std::chrono::high_resolution_clock::now() - startTime < duration) && (packets < TEMP_PACKET_LIMIT)) {
            size_t n = rx_stream->recv(buffs, spb, md, 5);
            if (md.out_of_sequence) {
                fprintf(stderr, "*** OUT OF SEQUENCE PACKET:\n%s\n***\n", md.to_pp_string(false).c_str());
                errors++;
            }
            if (!n) {
                fprintf(stderr, "*** No bytes received:\n%s\n***\n", md.to_pp_string(false).c_str());
                errors++;
            } else {
                packets++;
            }
            bytes += n;
        }
        auto finishTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - startTime);
        bytes *= 4; /* bytes = samples * 4 */
        bytes += 16 * packets; /* Account for CHDR */
        double megabits_per_second = (((double) bytes / total_time) / (1024 * 1024)) * 8;
        printf("RESULT duration ms:%ld packets:%lu bytes:%lu Mb/s:%f errors:%lu\n",
               finishTime.count(), packets, bytes, megabits_per_second, errors);
    }
};


int IHD_SAFE_MAIN(int argc, char *argv[]) {
    std::cout << "Revision: " << ihd::get_version_string() << std::endl;
    std::string args;
    size_t chan_mask;
    uint32_t total_time;

    std::string dest_ip;
    uint16_t dest_port;
    uint32_t fft_size;
    uint32_t fft_avg;
    std::string stream_type;

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "help message")
            ("duration", po::value<uint32_t>(&total_time)->default_value(30), "total number of seconds to receive")
            ("chan_mask", po::value<size_t>(&chan_mask)->default_value(1),
             "channel mask (chan 1 = 0x1, chan 2 = 0x2, chan 1 & 2 = 0x3)")
            ("dest_ip", po::value<std::string>(&dest_ip)->default_value("0.0.0.0"), "destination IP address")
            ("dest_port", po::value<uint16_t>(&dest_port)->default_value(9090), "destination port")
            ("fft_size", po::value<uint32_t>(&fft_size)->default_value(256), "FFT size (256, 512, 1024, 2048 or 4096")
            ("fft_avg", po::value<uint32_t>(&fft_avg)->default_value(105), "FFT averaging count")
            ("args", po::value<std::string>(&args)->default_value(""), "ISRP device address args")
            ("stream_type", po::value<std::string>(&stream_type)->default_value("psd"), "Stream type - (psd or iq)");
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

    std::vector<std::thread *> thread_vector;
    std::vector<RxStream *> stream_vector;
    int chan = 1;

    while (chan_mask != 0) {
        size_t current_mask = 1 << (chan - 1);
        if (current_mask & chan_mask) {
            stream_args.args[ihd::ipsolon_rx_stream::stream_type::STREAM_DEST_PORT_KEY] = std::to_string(dest_port);
            std::thread *thread_obj;
            RxStream *rxStream;
            if (stream_type == ihd::ipsolon_rx_stream::stream_type::PSD_STREAM) {
                rxStream = new RxStreamPsd(chan, isrp, total_time, stream_args);
                thread_obj = new std::thread(&RxStream::stream_run, rxStream);
            } else {
                rxStream = new RxStreamIq(chan, isrp, total_time, stream_args);
                thread_obj = new std::thread(&RxStream::stream_run, rxStream);
            }
            thread_vector.push_back(thread_obj);
            stream_vector.push_back(rxStream);
            chan_mask &= ~(current_mask);
            dest_port++;
        }
        chan++;
        if (chan > ihd::ipsolon_rx_stream::MAX_RX_CHANNELS) {
            chan_mask = 0;
        }
    }

    for (auto t: thread_vector) {
        t->join();
        free(t);
    }
    for (auto s: stream_vector) {
        free(s);
    }
    return (0);
}
