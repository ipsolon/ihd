/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#include <iostream>
#include <sys/socket.h>

#include <uhd/transport/udp_simple.hpp>

#include "chameleon_fw_common.hpp"
#include "chameleon_rx_stream_psd.hpp"
#include "chameleon_packet.hpp"
using namespace ihd;

//FIXME - need to figure out buffer sizes
#define PSD_STREAM_BUFFER_SIZE  (4 * 1024 * 1024)

chameleon_rx_stream_psd::chameleon_rx_stream_psd(const uhd::stream_args_t &stream_cmd,
                                         const uhd::device_addr_t &device_addr) :
                        chameleon_rx_stream(stream_cmd, device_addr)
 {
    _vita_port_timeout = {DEFAULT_PSD_TIMEOUT, 0};

    if (stream_cmd.args.has_key(ipsolon_rx_stream::stream_type::FFT_SIZE_KEY)) {
        std::string fft_size = stream_cmd.args[ipsolon_rx_stream::stream_type::FFT_SIZE_KEY];
        _fft_size = std::strtol(fft_size.c_str(), nullptr, 10);
    }
    if (stream_cmd.args.has_key(ipsolon_rx_stream::stream_type::FFT_AVG_COUNT_KEY)) {
        std::string fft_avg = stream_cmd.args[ipsolon_rx_stream::stream_type::FFT_AVG_COUNT_KEY];
        _fft_avg = std::strtol(fft_avg.c_str(), nullptr, 10);
    }
    _bytes_per_packet = (_fft_size * BYTES_PER_IQ_PAIR) + PACKET_HEADER_SIZE;
    // FIXME - fix buffering? Need to speed up udp
    _buffer_mem_size = (PSD_STREAM_BUFFER_SIZE); /* The memory allocated to store received UDP packets */

    _max_samples_per_packet = (_bytes_per_packet - PACKET_HEADER_SIZE) / BYTES_PER_IQ_PAIR;
    _buffer_packet_cnt = _buffer_mem_size / _max_samples_per_packet;

    /* Fill the free queue */
    std::lock_guard<std::mutex> lock(mtx_free_queue);
    for (int i = 0; i < _buffer_packet_cnt; ++i) {
        auto cp = new chameleon_packet(_bytes_per_packet);
        q_free_packets.push(cp);
    }

    send_rx_cfg_set_cmd(_chanMask);

}

void chameleon_rx_stream_psd::send_rx_cfg_set_cmd(const uint32_t chanMask) {
    size_t chan_num = 1;
    for (int i = 0; i < MAX_RX_CHANNELS; i++) {
        size_t chan_enabled = chanMask & (1 << i);
        if (chan_enabled) {
            std::unique_ptr<chameleon_fw_cmd> rx_cfg_set_cmd(new chameleon_fw_rx_cfg_set(chan_num,
                                                             ipsolon_rx_stream::stream_type::PSD_STREAM,
                                                            _fft_size,
                                                            _fft_avg));
            chameleon_fw_comms chameleon_fw_rx_cfg_set(std::move(rx_cfg_set_cmd));
            _commander.send_request(chameleon_fw_rx_cfg_set);
        }
        ++chan_num;
    }
}

