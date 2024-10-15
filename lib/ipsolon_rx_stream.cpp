/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "ipsolon_rx_stream.hpp"
#include "chameleon_rx_stream.hpp"
#include "exception.hpp"

using namespace ihd;

size_t ipsolon_rx_stream::get_num_channels() const
{
    THROW_NOT_IMPLEMENTED_ERROR();
}

size_t ipsolon_rx_stream::get_max_num_samps() const
{
    THROW_NOT_IMPLEMENTED_ERROR();
}

size_t ipsolon_rx_stream::recv(const buffs_type& buffs, const size_t nsamps_per_buff, uhd::rx_metadata_t& metadata,
                               const double timeout, const bool one_packet)
{
    THROW_NOT_IMPLEMENTED_ERROR();
}

void ipsolon_rx_stream::issue_stream_cmd(const uhd::stream_cmd_t& stream_cmd)
{
    THROW_NOT_IMPLEMENTED_ERROR();
}

ipsolon_rx_stream::sptr ipsolon_rx_stream::make(const uhd::stream_args_t& stream_cmd, const uhd::device_addr_t& device_addr)
{
    // There is only one option right now
    return std::make_shared<chameleon_rx_stream>(stream_cmd, device_addr);
}