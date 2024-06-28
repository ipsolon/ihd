/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef IPSOLON_STREAM_HPP
#define IPSOLON_STREAM_HPP

#include <uhd/stream.hpp>

namespace ihd {

class ipsolon_stream : public uhd::rx_streamer {

public:
    ipsolon_stream() {}
    typedef std::shared_ptr<ipsolon_stream> sptr;

    size_t get_num_channels() const override;
    size_t get_max_num_samps() const override;
    size_t recv(const buffs_type& buffs, const size_t nsamps_per_buff, uhd::rx_metadata_t& metadata,
        const double timeout, const bool one_packet) override;
    void issue_stream_cmd(const uhd::stream_cmd_t& stream_cmd) override;

    static sptr make(const uhd::stream_args_t& stream_cmd, const uhd::device_addr_t& _device_addr);
};

} // ihd

#endif //IPSOLON_STREAM_HPP
