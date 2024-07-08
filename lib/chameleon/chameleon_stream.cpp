/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <uhd/transport/udp_simple.hpp>

#include "chameleon_fw_common.h"
#include "chameleon_stream.hpp"
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
    uint8_t vita_buff[max_sample_per_packet] = {0};
    sockaddr_in server_addr{};
    socklen_t len;
    ssize_t n = 0;

    if (timeout != _vita_port_timeout.tv_sec) {
        _vita_port_timeout.tv_sec = timeout;
        err = setsockopt(_socket_fd, SOL_SOCKET, SO_RCVTIMEO, &_vita_port_timeout, sizeof(_vita_port_timeout));
        if (err < 0) {
            perror("Socket timeout set error");
        }
    }
    if (!err) {
        n = recvfrom(_socket_fd, &vita_buff, sizeof(vita_buff),0,(struct sockaddr *) &server_addr,&len);
        if (n > 0) {
            std::cout << "Received bytes:" << n << std::endl;
            uint64_t header = vita_buff[7] |
                              vita_buff[6] << 8  |
                              vita_buff[5] << 16 |
                              vita_buff[4] << 24 |
                    (uint64_t)vita_buff[3] << 32 |
                    (uint64_t)vita_buff[2] << 40 |
                    (uint64_t)vita_buff[1] << 48 |
                    (uint64_t)vita_buff[0] << 56;
            printf("Header: 0x%016lx\n", header);
            uint16_t dst_EPID =  vita_buff[1] << 8 | vita_buff[0];
            uint16_t length   =  vita_buff[3] << 8 | vita_buff[2];
            uint16_t seq_num  =  vita_buff[5] << 8 | vita_buff[4];
            uint8_t  num_data =  vita_buff[6] & 0x1F;
            uint8_t  pkt_type = (vita_buff[6] & 0xE0) >> 5;
            uint8_t  eov      =  vita_buff[7] & 0x01;
            uint8_t  eob      = (vita_buff[7] & 0x02) >> 1;
            uint8_t  vc       = (vita_buff[7] & 0xFC) >> 2;
            printf("EPID:%d Length:%d SeqNum:%d NumData:%d PktType:%d EOV:%d EOB:%d VC:%d\n",
                   dst_EPID, length, seq_num, num_data, pkt_type, eov, eob, vc);
        } else if (n < 0 && errno == ETIMEDOUT) {
            n = 0;
            std::cout << "Timeout waiting for data" << std::endl;
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
    auto request = chameleon_fw_comms_t();
    request.flags             =  CHAMELEON_FW_COMMS_FLAGS_WRITE;
    request.addr              = CHAMELEON_FW_COMMS_CMD_STREAM_CMD;
    request.stream.enable     = true;
    request.stream.chans      = _chanMask;

    _commander.send_request(request);
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
    err = socket(AF_INET, SOCK_DGRAM, 0);
    if (err < 0) {
        perror("socket creation failed");
    } else {
        sock_fd = err;
        err = 0;

        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(vita_port);
        server_addr.sin_addr.s_addr = INADDR_ANY;
        err = bind(sock_fd, (const struct sockaddr*)&server_addr, sizeof(server_addr));
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
