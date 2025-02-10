/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <thread>

#include "safe_main.hpp"
#include "ihd.h"

#include "debug.hpp"

namespace po = boost::program_options;

#define NUMBER_OF_CHANNELS          1 /* We are limited to a single channel per stream right now */

class RxStream {
public:
    RxStream(size_t chan, const ihd::ipsolon_isrp::sptr &isrp, uint32_t tt,
             uhd::stream_args_t &stream_args) : channel(chan), total_time(tt) {
        std::vector<size_t> channel_nums;
        channel_nums.push_back(chan);
        stream_args.channels = channel_nums;
        rx_stream = isrp->get_rx_stream(stream_args);
    }

    virtual ~RxStream() = default;

    // Start a threads to receive iq/psd data - one for each channel enabled in the chan_mask
    virtual void run_loop() = 0;

    static constexpr uint64_t BYTES_PER_CHDR = 16;

    void start_stream() const {
        std::cout << "start_stream stream for channel:" << channel << std::endl;

        /************************************************************************
         * Start the stream
         ***********************************************************************/
        uhd::stream_cmd_t const stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
        rx_stream->issue_stream_cmd(stream_cmd);
    }

    void stop_stream() const {
        /************************************************************************
         * Stop the stream
         ***********************************************************************/
        uhd::stream_cmd_t const stream_cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
        rx_stream->issue_stream_cmd(stream_cmd);
    }

private:
    uhd::stream_args_t stream_args{};

protected:
    size_t channel{};
    uint32_t total_time{};
    uhd::rx_streamer::sptr rx_stream{};
};

class RxStreamPsd : public RxStream {
public:
    RxStreamPsd(size_t chan, const ihd::ipsolon_isrp::sptr &isrp, uint32_t tt, uhd::stream_args_t &stream_args)
        : RxStream(chan, isrp, tt, stream_args) {
    }

    ~RxStreamPsd() override = default;

private:
    void run_loop() override {
        uhd::rx_metadata_t md;
        /************************************************************************
         * Allocate buffers
         ***********************************************************************/
        std::vector<std::complex<int16_t> *> buffs(NUMBER_OF_CHANNELS);
        size_t const spb = rx_stream->get_max_num_samps();
        std::complex<int16_t> p[spb];
        buffs[0] = p;
        auto startTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::seconds(total_time);

        uint64_t errors = 0;
        uint64_t packets = 0;
        uint64_t bytes = 0;
        while (std::chrono::high_resolution_clock::now() - startTime < duration) {
            size_t const n = rx_stream->recv(buffs, spb, md, 5);
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
        bytes += BYTES_PER_CHDR * packets; /* Account for CHDR */
        double const megabits_per_second = ((static_cast<double>(bytes) / total_time) / (1024 * 1024)) * 8;
        printf("RESULT chan:%zu duration ms:%ld packets:%lu bytes:%lu Mb/s:%f errors:%lu\n",
               channel, finishTime.count(), packets, bytes, megabits_per_second, errors);
    }
};

class RxStreamIq : public RxStream {
public:
    RxStreamIq(size_t chan, const ihd::ipsolon_isrp::sptr &isrp, uint32_t tt, uhd::stream_args_t &stream_args)
        : RxStream(chan, isrp, tt, stream_args) {
    }

    ~RxStreamIq() override = default;

private:
    void run_loop() override {
        uhd::rx_metadata_t md;
        /************************************************************************
         * Allocate buffers
         ***********************************************************************/
        std::vector<std::complex<int16_t> *> buffs(NUMBER_OF_CHANNELS);
        size_t const spb = rx_stream->get_max_num_samps();
        std::complex<int16_t> p[spb];
        buffs[0] = p;
        auto startTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::seconds(total_time);

        uint64_t errors = 0;
        uint64_t packets = 0;
        uint64_t bytes = 0;
        // FIXME - TEMP_PACKET_LIMIT is temporary until udp keeps up with dma
        while ((std::chrono::high_resolution_clock::now() - startTime < duration) && (packets < TEMP_PACKET_LIMIT)) {
            size_t const n = rx_stream->recv(buffs, spb, md, 5);
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
        bytes += BYTES_PER_CHDR * packets; /* Account for CHDR */
        double const megabits_per_second = ((static_cast<double>(bytes) / total_time) / (1024 * 1024)) * 8;
        printf("RESULT chan:%zu duration ms:%ld packets:%lu bytes:%lu Mb/s:%f errors:%lu\n",
               channel, finishTime.count(), packets, bytes, megabits_per_second, errors);
    }
};


int IHD_SAFE_MAIN(int argc, char *argv[]) {
    std::cout << "Revision: " << ihd::get_version_string() << std::endl;
    constexpr int DEFAULT_NUM_LOOPS = 5;
    constexpr uint16_t DEFAULT_PORT = 9090;

    std::string args;
    constexpr uint32_t total_time = 2;
    uint32_t num_loops = 2;

    std::string dest_ip;
    uint16_t dest_port = 0;
    constexpr uint32_t fft_size = 256;
    constexpr uint32_t fft_avg = 105;
    std::string const stream_type;

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "help message")
            ("num_loops", po::value<uint32_t>(&num_loops)->default_value(DEFAULT_NUM_LOOPS),
             "Number of start stop loops")
            ("dest_ip", po::value<std::string>(&dest_ip)->default_value("0.0.0.0"), "destination IP address")
            ("dest_port", po::value<uint16_t>(&dest_port)->default_value(DEFAULT_PORT), "destination port")
            ("args", po::value<std::string>(&args)->default_value(""), "USRP device address args");
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
        return 0;
    }
    if (vm["args"].defaulted()) {
        std::cout
                << std::endl
                << "ERROR 'args' is a mandatory parameter please specify the chameleon ip address, e.g. args=addr=10.75.42.209 "
                << std::endl
                << boost::format("IHD RX samples to file %s") % desc << std::endl;
        return -1;
    }

    /************************************************************************
     * Create device and block controls
     ***********************************************************************/
    std::cout << std::endl;
    std::cout << boost::format("Creating the ISRP device with: %s...") % args << std::endl;

    ihd::ipsolon_isrp::sptr const isrp = ihd::ipsolon_isrp::make(args);

    std::vector<std::thread *> const thread_vector;
    std::vector<RxStream *> const stream_vector;
    // Create four streams - two iq and two psd

    // First IQ stream
    uhd::stream_args_t stream_args1("sc16", "sc16");
    stream_args1.args[ihd::ipsolon_rx_stream::stream_type::STREAM_FORMAT_KEY] =
            ihd::ipsolon_rx_stream::stream_type::IQ_STREAM;
    stream_args1.args[ihd::ipsolon_rx_stream::stream_type::STREAM_DEST_IP_KEY] =
            dest_ip;
    stream_args1.args[ihd::ipsolon_rx_stream::stream_type::STREAM_DEST_PORT_KEY] =
            std::to_string(dest_port);

    stream_args1.args[ihd::ipsolon_rx_stream::stream_type::STREAM_DEST_PORT_KEY] = std::to_string(dest_port);
    RxStream *rxStream1 = nullptr;
    rxStream1 = new RxStreamIq(1, isrp, total_time, stream_args1);

    // Second IQ stream
    uhd::stream_args_t stream_args2("sc16", "sc16");
    stream_args2.args[ihd::ipsolon_rx_stream::stream_type::STREAM_FORMAT_KEY] =
            ihd::ipsolon_rx_stream::stream_type::IQ_STREAM;
    stream_args2.args[ihd::ipsolon_rx_stream::stream_type::STREAM_DEST_IP_KEY] =
            dest_ip;
    stream_args2.args[ihd::ipsolon_rx_stream::stream_type::STREAM_DEST_PORT_KEY] =
            std::to_string(dest_port + 1);

    stream_args2.args[ihd::ipsolon_rx_stream::stream_type::STREAM_DEST_PORT_KEY] = std::to_string(dest_port);
    RxStream *rxStream2 = nullptr;
    rxStream2 = new RxStreamIq(2, isrp, total_time, stream_args2);

    // Third stream is PSD
    uhd::stream_args_t stream_args3("sc16", "sc16");
    stream_args3.args[ihd::ipsolon_rx_stream::stream_type::STREAM_FORMAT_KEY] =
            ihd::ipsolon_rx_stream::stream_type::PSD_STREAM;
    stream_args3.args[ihd::ipsolon_rx_stream::stream_type::STREAM_DEST_IP_KEY] =
            dest_ip;
    stream_args3.args[ihd::ipsolon_rx_stream::stream_type::STREAM_DEST_PORT_KEY] =
            std::to_string(dest_port + 2);
    stream_args3.args[ihd::ipsolon_rx_stream::stream_type::FFT_SIZE_KEY] =
            std::to_string(fft_size);
    stream_args3.args[ihd::ipsolon_rx_stream::stream_type::FFT_AVG_COUNT_KEY] =
            std::to_string(fft_avg);
    stream_args3.args[ihd::ipsolon_rx_stream::stream_type::STREAM_DEST_PORT_KEY] = std::to_string(dest_port);
    RxStream *rxStream3 = nullptr;
    rxStream3 = new RxStreamPsd(3, isrp, total_time, stream_args3);

    // Fourth stream is PSD
    uhd::stream_args_t stream_args4("sc16", "sc16");
    stream_args4.args[ihd::ipsolon_rx_stream::stream_type::STREAM_FORMAT_KEY] =
            ihd::ipsolon_rx_stream::stream_type::PSD_STREAM;
    stream_args4.args[ihd::ipsolon_rx_stream::stream_type::STREAM_DEST_IP_KEY] =
            dest_ip;
    stream_args4.args[ihd::ipsolon_rx_stream::stream_type::STREAM_DEST_PORT_KEY] =
            std::to_string(dest_port + 3);
    stream_args4.args[ihd::ipsolon_rx_stream::stream_type::FFT_SIZE_KEY] =
            std::to_string(fft_size);
    stream_args4.args[ihd::ipsolon_rx_stream::stream_type::FFT_AVG_COUNT_KEY] =
            std::to_string(fft_avg);

    stream_args4.args[ihd::ipsolon_rx_stream::stream_type::STREAM_DEST_PORT_KEY] = std::to_string(dest_port);
    RxStream *rxStream4 = nullptr;
    rxStream4 = new RxStreamPsd(4, isrp, total_time, stream_args4);

    // Fourth stream is PSD
    for (int i = 0; i < num_loops; i++) {
        constexpr int ONE_SECOND = 1000000;
        rxStream1->start_stream();
        rxStream2->start_stream();
        rxStream3->start_stream();
        rxStream4->start_stream();
        usleep(ONE_SECOND);
        rxStream1->stop_stream();
        rxStream2->stop_stream();
        rxStream3->stop_stream();
        rxStream4->stop_stream();
    }

    delete rxStream1;
    delete rxStream2;
    delete rxStream3;
    delete rxStream4;

    return (0);
}
