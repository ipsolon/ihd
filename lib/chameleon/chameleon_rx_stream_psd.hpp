/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/


#ifndef CHAMELEON_STREAM_PSD_HPP
#define CHAMELEON_STREAM_PSD_HPP

#include "chameleon_rx_stream.hpp"

//FIXME - need to figure out buffer sizes
#define PSD_STREAM_BUFFER_SIZE  (4 * 1024 * 1024)

namespace ihd {
    class chameleon_packet;

    class chameleon_rx_stream_psd : public chameleon_rx_stream {
    public:
        static constexpr size_t DEFAULT_PSD_TIMEOUT = 30;
        explicit chameleon_rx_stream_psd(const uhd::stream_args_t &stream_cmd, const uhd::device_addr_t &device_addr);

        ~chameleon_rx_stream_psd() override;

    protected:
        void send_rx_cfg_set_cmd(uint32_t chanMask) override;
        size_t get_max_num_samps() const override {
            return _max_samples_per_packet;
        }

    private:
        static constexpr uint32_t DEFAULT_PACKET_SIZE = 8192;
        static constexpr uint32_t DEFAULT_FFT_SIZE = 256;
        static constexpr uint32_t DEFAULT_FFT_AVG = 120;

        uint32_t _fft_size;
        uint32_t _fft_avg;

        size_t _max_samples_per_packet;

        size_t _bytes_per_packet = DEFAULT_PACKET_SIZE;
        // FIXME - fix buffering? Need to speed up udp
        size_t _buffer_mem_size{PSD_STREAM_BUFFER_SIZE}; /* The memory allocated to store received UDP packets */

        size_t _buffer_packet_cnt{0};

        timeval _vita_port_timeout = {DEFAULT_PSD_TIMEOUT, 0};

    };
} // ihd

#endif //CHAMELEON_STREAM_PSD_HPP
