/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "ipsolon_rx_stream.hpp"
#include "chameleon_rx_stream.hpp"
#include "exception.hpp"

using namespace ihd;

ipsolon_rx_stream::sptr ipsolon_rx_stream::make(const uhd::stream_args_t& stream_cmd, const uhd::device_addr_t& device_addr)
{
    // There is only one option right now
    return std::make_shared<chameleon_rx_stream>(stream_cmd, device_addr);
}