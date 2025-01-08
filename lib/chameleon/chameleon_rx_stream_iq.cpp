/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <uhd/transport/udp_simple.hpp>

#include "chameleon_fw_common.hpp"
#include "chameleon_rx_stream_iq.hpp"
#include "chameleon_packet.hpp"
#include <exception.hpp>
#include "debug.hpp"

using namespace ihd;

chameleon_rx_stream_iq::chameleon_rx_stream_iq(const uhd::stream_args_t &stream_cmd,
                                         const uhd::device_addr_t &device_addr) :
                        chameleon_rx_stream(stream_cmd, device_addr)
 {
    _vita_port_timeout = {0, DEFAULT_USEC_TIMEOUT};
    _packet_size = DEFAULT_PACKET_SIZE;
    _bytes_per_packet = DEFAULT_PACKET_SIZE;
    _buffer_mem_size = DEFAULT_IQ_BUFFER_MEM_SIZE;
    _max_samples_per_packet = (_bytes_per_packet - PACKET_HEADER_SIZE) / BYTES_PER_IQ_PAIR;
    _buffer_packet_cnt = _buffer_mem_size / _max_samples_per_packet;

    /* Fill the free queue */
    std::lock_guard<std::mutex> lock(mtx_free_queue);
    for (int i = 0; i < _buffer_packet_cnt; ++i) {
        auto cp = new chameleon_packet(_bytes_per_packet);
        q_free_packets.push(cp);
    }
}

chameleon_rx_stream_iq::~chameleon_rx_stream_iq() {
}

void chameleon_rx_stream_iq::send_rx_cfg_set_cmd(uint32_t chanMask) {
    size_t chan_num = 1;
    for (int i = 0; i < MAX_RX_CHANNELS; i++) {
        size_t chan_enabled = chanMask & (1 << i);
        if (chan_enabled) {
            std::unique_ptr<chameleon_fw_cmd> rx_cfg_set_cmd(new chameleon_fw_rx_cfg_set(chan_num, ipsolon_rx_stream::stream_type::IQ_STREAM,
                                                       0, /* fft_size unused */
                                                       0 /* fft_avg unused */,
                                                       _packet_size));
            chameleon_fw_comms chameleon_fw_rx_cfg_set(std::move(rx_cfg_set_cmd));
            _commander.send_request(chameleon_fw_rx_cfg_set);
        }
        ++chan_num;
    }
}

