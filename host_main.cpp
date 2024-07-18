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


#define NUMBER_OF_CHANNELS 1 /* We are limited to a single channel right now */
#define DEFAULT_UDP_PACKET_SIZE    64000
#define DEFAULT_BYTES_PER_SAMPLE       4 /* 16 Bit I/Q = 4 Bytes */
#define DEFAULT_SAMPLE_PER_BUFFER (DEFAULT_UDP_PACKET_SIZE / DEFAULT_BYTES_PER_SAMPLE)

/**
 * Do a ramp check on the file, assumes
 * @param fd open file descriptor to samples file
 * @param buffer_size Each 'buffer size' chunk is expected to start with packet number and then ramp up,
 *                      from 1 to 'buffer size' - 1.
 */
static void rampcheck(int fd, size_t buffer_size)
{
    ssize_t  read_bytes = 0;
    size_t n_buffs = 0;
    size_t IQ_count = buffer_size / DEFAULT_BYTES_PER_SAMPLE; // How many IQ pairs per buffer

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
    if (!err && n_buffs) {
        for (int iteration = 0; iteration < n_buffs; iteration++) {
            uint16_t buff[buffer_size / 2];
            ssize_t n = read(fd, buff, buffer_size);
            if (n != buffer_size) {
                fprintf(stderr, "Error reading from file:%s", strerror(errno));
            } else {
                read_bytes += n;
                uint16_t packet_count1 = buff[0];
                uint16_t packet_count2 = buff[1];
                uint16_t ramp = 1;
                for (int j = 2; j < (IQ_count * 2); j += 2) {
                    uint16_t i = buff[j];
                    uint16_t q = buff[j + 1];
                    if (i != q) {
                        printf("\nI/Q mismatch: i:%04x q:%04x sample:%d\n", i, q, j);
                    }
                    if (i != ramp || q != ramp) {
                        printf("\nRamp pattern mismatch: expected:%02x got i:%02x q:%02x sample:%d\n", ramp, i, q, j);
                    }
                    ramp++;
                    printf("Read bytes:%ld Packet Count:%x:%x i:%04x q:%04x\r",
                           read_bytes, packet_count1, packet_count2, i, q);
                }
            }
        }
        printf("\n\n");
    }
}

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
        ("spb", po::value<size_t>(&spb)->default_value(DEFAULT_SAMPLE_PER_BUFFER), "samples per buffer")
        ("freq", po::value<double>(&freq)->default_value(0.0), "RF center frequency in Hz")
        ("channel", po::value<size_t>(&channel)->default_value(0), "which channel to use")
        ("args", po::value<std::string>(&args)->default_value(""), "ISRP device address args")
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
    uint8_t p[DEFAULT_UDP_PACKET_SIZE];
    buffs[0] = p;

    int fd = open(file.c_str(), O_CREAT | O_TRUNC | O_RDWR,
                                        S_IRUSR | S_IWUSR | S_IRGRP |
                                        S_IWGRP | S_IROTH | S_IWOTH);
    if (fd < 0) {
        perror("File Open error");
        exit(fd);
    }

    uhd::rx_metadata_t md;
    for (int i = 0; i < 900; i++) {
        size_t n = rx_stream->recv(buffs, DEFAULT_UDP_PACKET_SIZE, md, 5);
        ssize_t w = write(fd, buffs[0], n);
        if (w != n) {
            fprintf(stderr, "Write failed. Request %lu bytes written, write returned:%lu. %s",
                    n, w, strerror(errno));
        }
    }

    if (vm.count("rampcheck")) {
        rampcheck(fd, DEFAULT_UDP_PACKET_SIZE);
    }
    stream_cmd.stream_mode = uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS;
    rx_stream->issue_stream_cmd(stream_cmd);
    close(fd);
    exit(0);
}
