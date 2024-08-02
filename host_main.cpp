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

#define NUMBER_OF_CHANNELS                1 /* We are limited to a single channel right now */
#if 0
#define DEFAULT_UDP_PACKET_SIZE       64000 /* Fixed size by radio */
#define DEFAULT_BYTES_PER_SAMPLE          2 /* 16 Bits */
#define DEFAULT_BYTES_PER_IQ_PAIR      (DEFAULT_BYTES_PER_SAMPLE * 2)   /* 16 Bit I/Q = 4 Bytes */
#define DEFAULT_IQ_SAMPLES_PER_BUFFER ((DEFAULT_UDP_PACKET_SIZE - 16) / \
                                        DEFAULT_BYTES_PER_IQ_PAIR)      /* i.e. the number of IQ pairs, minus CHDR & timestamp */
#endif

/**
 * Do a ramp check on the file, assumes
 * @param fd open file descriptor to samples file
 * @param buffer_size Each 'buffer size' chunk is expected to start with packet number and then ramp up,
 *                      from 1 to 'buffer size' - 1.
 */

static bool first_check = true;
static uint16_t next_expect_ramp = 0;
static void rampcheck(int fd, size_t buffer_size)
{
    ssize_t  read_bytes = 0;
    size_t n_buffs = 0;
    size_t IQ_pairs = buffer_size / ihd::ipsolon_stream::BYTES_PER_SAMPLE; // How many IQ pairs per buffer

    int err = lseek(fd, 0, SEEK_SET);
    if (!err) {
        struct stat stat_buf{};
        err = fstat(fd, &stat_buf);
        if (err) {
            fprintf(stderr, "Error getting stat on file:%s", strerror(errno));
        } else {
            n_buffs = (stat_buf.st_size / buffer_size);
        }
    }
    uint16_t ramp = 0;
    if (!err && n_buffs) {
        for (int iteration = 0; iteration < n_buffs; iteration++) {
            uint16_t buff[buffer_size / 2];
            ssize_t n = read(fd, buff, buffer_size);
            if (n != buffer_size) {
                fprintf(stderr, "Error reading from file:%s", strerror(errno));
            } else {
                read_bytes += n;
                uint16_t ramp_count1 = buff[0];
                uint16_t ramp_count2 = buff[1];
                uint16_t ramp_count3 = buff[2];
                uint16_t ramp_count4 = buff[3];

                if (ramp_count1 == ramp_count2 &&
                    ramp_count2 == ramp_count3 &&
                    ramp_count3 == ramp_count4)
                {
                    if (first_check) {
                        ramp = ramp_count1 + 1;
                    } else {
                        if (ramp_count1 != next_expect_ramp) {
                            fprintf(stderr, "Dropped packet possible, expected:0x%02x got:0x%02x diff:0x%02x\n",
                                    next_expect_ramp, ramp_count1, ramp_count1 - next_expect_ramp);
                        }
                    }
                } else {
                    fprintf(stderr, "Leading ramp count failed:%02x:%02x:%02x:%02x\n",
                            ramp_count1, ramp_count2, ramp_count3, ramp_count4);
                }

                for (int j = 4; j < IQ_pairs && !err; j += 2) {
                    uint16_t i = buff[j];
                    uint16_t q = buff[j + 1];
                    if (i != q) {
                        err = -1;
                        size_t fpos = (lseek(fd, 0, SEEK_CUR) - buffer_size) + (j * sizeof(uint16_t));
                        printf("\nI/Q mismatch: i:0x%04x q:0x%04x sample:0x%x file offset:0x%lx\n",
                               i, q, j, fpos);
                    }
                    if (i != ramp || q != ramp) {
                        err = -1;
                        size_t fpos = (lseek(fd, 0, SEEK_CUR) - buffer_size) + (j * sizeof(uint16_t));
                        printf("\nRamp pattern mismatch: expected:%02x got i:%02x q:%02x sample:%x file offset:0x%lx\n",
                               ramp, i, q, j, fpos);
                    }
                    ramp++;
                    printf("Read bytes:%ld i:%04x q:%04x\r", read_bytes, i, q);
                }
            }
        }
        next_expect_ramp = ramp - 1;  // Then next packet should lead with the last value of the previous packet
        printf("\n\n");
    }
}

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
        ("nsamps", po::value<size_t>(&total_num_samps)->default_value(ihd::ipsolon_stream::SAMPLES_PER_PACKET * 10), "total number of samples to receive")
        ("spb", po::value<size_t>(&spb)->default_value(ihd::ipsolon_stream::SAMPLES_PER_PACKET), "samples per buffer")
        ("freq", po::value<double>(&freq)->default_value(0.0), "RF center frequency in Hz")
        ("channel", po::value<size_t>(&channel)->default_value(0), "which channel to use")
        ("args", po::value<std::string>(&args)->default_value(""), "ISRP device address args")
        ("fft","Stream FFTs (versus an I/Q stream)")
        ("rampcheck","Do ramp check on the file after collecting samples")
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
    std::vector<std::complex<uint16_t>*> buffs(NUMBER_OF_CHANNELS);
    std::complex<uint16_t> p[spb];
    buffs[0] = p;

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

    /************************************************************************
     * Get Rx Stream
     ***********************************************************************/
    uhd::stream_args_t stream_args("sc16", "sc16");
    std::vector<size_t> channel_nums;
    channel_nums.push_back(channel);
    stream_args.channels = channel_nums;

    if (vm.count("fft")) {
        stream_args.args[ihd::ipsolon_stream::stream_type::STREAM_FORMAT_KEY] =
                         ihd::ipsolon_stream::stream_type::FFT_STREAM;
    } else {
        stream_args.args[ihd::ipsolon_stream::stream_type::STREAM_FORMAT_KEY] =
                ihd::ipsolon_stream::stream_type::IQ_STREAM;
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
    for (size_t i = 0; i < sample_iterations; i++) {
        size_t n = rx_stream->recv(buffs, spb, md, 5);
        size_t ws = n *  ihd::ipsolon_stream::BYTES_PER_IQ_PAIR;
        ssize_t w = write(fd, buffs[0], ws);
        if (w != ws) {
            fprintf(stderr, "Write failed. Request %lu bytes written, write returned:%lu. %s\n",
                    n, w, strerror(errno));
        }
    }
    stream_cmd.stream_mode = uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS;
    rx_stream->issue_stream_cmd(stream_cmd);

    if (vm.count("rampcheck")) {
        rampcheck(fd, spb * ihd::ipsolon_stream::BYTES_PER_IQ_PAIR);
    }
    close(fd);
    return(0);
}
