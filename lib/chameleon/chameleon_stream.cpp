/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>

#include <uhd/transport/udp_simple.hpp>

#include "chameleon_fw_common.h"
#include "chameleon_stream.hpp"
#include "chameleon_chdr_header.h"
#include "exception.hpp"

using namespace ihd;

chameleon_stream::chameleon_stream(const uhd::stream_args_t& stream_cmd, const uhd::device_addr_t& device_addr) :
    _nChans(stream_cmd.channels.size()), _commander(device_addr), _chanMask(0)
{
    if(stream_cmd.cpu_format != "sc16") {
        THROW_VALUE_NOT_SUPPORTED_ERROR(stream_cmd.args.to_string());
    }
    if(stream_cmd.otw_format != "sc16") {
        THROW_VALUE_NOT_SUPPORTED_ERROR(stream_cmd.args.to_string());
    }
    for(const size_t& chan : stream_cmd.channels) {
        _chanMask |= 1 << chan; /* Channels indexed at zero */
    }
    open_socket();
}

size_t chameleon_stream::get_num_channels() const
{
    return _nChans;
}

size_t chameleon_stream::get_max_num_samps() const
{
    return max_sample_per_packet;
}

size_t chameleon_stream::recv(const buffs_type& buffs, const size_t nsamps_per_buff, uhd::rx_metadata_t& metadata,
    const double timeout, const bool one_packet)
{
    int err = 0;
    uint8_t vita_buff[bytes_per_packet] = {0};
    sockaddr_in server_addr{};
    socklen_t len;
    ssize_t n = 0;

    double vita_to = static_cast<double>(_vita_port_timeout.tv_sec) +
                     static_cast<double>(_vita_port_timeout.tv_usec) / 1000000.0;
    if (timeout != vita_to) {
        _vita_port_timeout.tv_sec = static_cast<__time_t>(timeout);
        _vita_port_timeout.tv_usec = static_cast<__suseconds_t>(std::fmod(timeout * 1000000, 1000000));
        err = setsockopt(_socket_fd, SOL_SOCKET, SO_RCVTIMEO, &_vita_port_timeout, sizeof(_vita_port_timeout));
        if (err < 0) {
            perror("Socket timeout set error");
        }
    }
    if (!err) {
        n = recvfrom(_socket_fd, &vita_buff, sizeof(vita_buff),0,(struct sockaddr *) &server_addr,&len);
        if (n > 0) {
            uint64_t header = vita_buff[0] |
                              vita_buff[1] << 8  |
                              vita_buff[2] << 16 |
                              vita_buff[3] << 24 |
                    (uint64_t)vita_buff[4] << 32 |
                    (uint64_t)vita_buff[5] << 40 |
                    (uint64_t)vita_buff[6] << 48 |
                    (uint64_t)vita_buff[7] << 56;

            chdr_header chdr(header);
            std::cout << "CHDR:" << chdr.to_string() << std::endl;

            void *p = buffs[0];
            n = std::min(nsamps_per_buff, (size_t)n);
            memcpy(p, vita_buff + 16 /* CHDR + Timestamp */, n);
        } else if (n < 0) {
            std::cout << "Receive error." << " errno:" << errno << ":" << strerror(errno) << std::endl;
        }
    }
    return n;
}

void chameleon_stream::issue_stream_cmd(const uhd::stream_cmd_t& stream_cmd)
{
    switch (stream_cmd.stream_mode) {
        case uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS : start_stream(); break;
        case uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS :  stop_stream(); break;
        case uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE :
        case uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_MORE :
        default:
            THROW_NOT_IMPLEMENTED_ERROR();
    }
}

void chameleon_stream::start_stream() const
{
#if IMPLEMENTED_CMD_PORT
    auto request = chameleon_fw_comms_t();
    request.flags             = CHAMELEON_FW_COMMS_FLAGS_WRITE;
    request.addr              = CHAMELEON_FW_COMMS_CMD_STREAM_CMD;
    request.stream.enable     = true;
    request.stream.chans      = _chanMask;
    _commander.send_request(request);
#else
    /* For now, you just send the radio 'anything' and it goes */
    sockaddr_in radio_addr{};
    radio_addr.sin_addr.s_addr = inet_addr("192.168.10.200");
    radio_addr.sin_port = htons(vita_port);

    uint8_t go[] = {0x67,0x6F};
    int n = sendto(_socket_fd, go, sizeof(go), MSG_CONFIRM,
                   (const struct sockaddr *) &radio_addr, sizeof(radio_addr));
    if (n != sizeof(go)) {
        THROW_SOCKET_ERROR();
    }
#endif
}

void chameleon_stream::stop_stream() const
{
    auto request = chameleon_fw_comms_t();
    request.flags             = CHAMELEON_FW_COMMS_FLAGS_WRITE;
    request.addr              = CHAMELEON_FW_COMMS_CMD_STREAM_CMD;
    request.stream.enable     = false;
    request.stream.chans      = _chanMask;

    _commander.send_request(request);
}

void chameleon_stream::open_socket() {
    int err = 0;
    int sock_fd = -1;

    // Creating socket file descriptor
    err = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (err < 0) {
        perror("socket creation failed");
    } else {
        int broadcast = 1;
        sock_fd = err;
        err = setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast);
        if (err) {
            perror("setsockopt failed");
        }
    }
    if (!err) {
        sockaddr_in local_addr{};
        local_addr.sin_family = AF_INET;
        local_addr.sin_port = htons(vita_port);
        local_addr.sin_addr.s_addr = INADDR_ANY;
        err = bind(sock_fd, (const struct sockaddr *) &local_addr, sizeof(local_addr));
        if (err < 0) {
            perror("bind failed");
        }
    }
    if (!err) {
        err = setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &_vita_port_timeout, sizeof(_vita_port_timeout));
        if (err < 0) {
            perror("Socket timeout set error");
        }
    }
    if (err) {
        if (sock_fd > -1) {
            close(sock_fd);
        }
        throw std::runtime_error("Error opening socket");
    } else {
        _socket_fd = sock_fd;
    }
}
