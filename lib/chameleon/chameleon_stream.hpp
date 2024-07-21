/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/


#ifndef CHAMELEON_STREAM_HPP
#define CHAMELEON_STREAM_HPP

#include <chameleon_fw_commander.hpp>

#include "ipsolon_stream.hpp"

namespace ihd {

class chameleon_stream : public ipsolon_stream {
public:
    static const size_t default_timeout = 5;

    explicit chameleon_stream(const uhd::stream_args_t& stream_cmd, const uhd::device_addr_t& device_addr);
    size_t get_num_channels() const override;
    size_t get_max_num_samps() const override;
    size_t recv(const buffs_type& buffs, const size_t nsamps_per_buff, uhd::rx_metadata_t& metadata,
        const double timeout, const bool one_packet) override;
    void issue_stream_cmd(const uhd::stream_cmd_t& stream_cmd) override;

private:
    static const int vita_port = 9090;
    static constexpr size_t bytes_per_sample = 4;
    static constexpr size_t bytes_per_packet = 64000;
    static constexpr size_t max_sample_per_packet = bytes_per_packet / bytes_per_sample;

    timeval _vita_port_timeout = {default_timeout, 0};

    size_t _nChans{};
    uint32_t _chanMask{};
    chameleon_fw_commander _commander;
    int _socket_fd{};

    void start_stream() const;
    void stop_stream() const;

    void open_socket();
};

} // ihd

#endif //CHAMELEON_STREAM_HPP
