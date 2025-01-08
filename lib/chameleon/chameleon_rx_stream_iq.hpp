/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/


#ifndef CHAMELEON_STREAM_IQ_HPP
#define CHAMELEON_STREAM_IQ_HPP
#include <chameleon_fw_commander.hpp>
#include <netinet/in.h>

#include "chameleon_rx_stream.hpp"

namespace ihd {
    class chameleon_packet;

    class chameleon_rx_stream_iq : public chameleon_rx_stream {
    public:
        static constexpr size_t DEFAULT_USEC_TIMEOUT = 500000;
        explicit chameleon_rx_stream_iq(const uhd::stream_args_t &stream_cmd, const uhd::device_addr_t &device_addr);

        ~chameleon_rx_stream_iq() override;

    protected:
        void send_rx_cfg_set_cmd(uint32_t chanMask) override;
        size_t get_max_num_samps() const {
            return _max_samples_per_packet;
        }

    private:
        static constexpr uint32_t DEFAULT_PACKET_SIZE = 8192;
        static constexpr uint32_t DEFAULT_IQ_BUFFER_MEM_SIZE = 0x28C58000;
        size_t _packet_size;
        size_t _bytes_per_packet = DEFAULT_PACKET_SIZE;
        size_t _buffer_mem_size = DEFAULT_IQ_BUFFER_MEM_SIZE;

        timeval _vita_port_timeout = {0, DEFAULT_USEC_TIMEOUT};

    };
} // ihd

#endif //CHAMELEON_STREAM_IQ_HPP
