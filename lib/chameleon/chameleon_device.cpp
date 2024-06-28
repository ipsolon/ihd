/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "chameleon_device.hpp"

#include <ipsolon_stream.hpp>

#include "exception.hpp"

ihd::chameleon_device::chameleon_device(const uhd::device_addr_t& _device_addr) : _device_addr(_device_addr) {}

ihd::chameleon_device::~chameleon_device() = default;

uhd::rx_streamer::sptr ihd::chameleon_device::get_rx_stream(const uhd::stream_args_t& args)
{
    return ipsolon_stream::make(args, _device_addr);
}

uhd::tx_streamer::sptr ihd::chameleon_device::get_tx_stream(const uhd::stream_args_t& args) { THROW_NOT_IMPLEMENTED_ERROR(); }

bool ihd::chameleon_device::recv_async_msg(uhd::async_metadata_t& async_metadata, double timeout) { THROW_NOT_IMPLEMENTED_ERROR(); }

uhd::device::sptr ihd::chameleon_device::chameleon_make(const uhd::device_addr_t& device_addr)
{
    return std::make_shared<ihd::chameleon_device>(device_addr);
}
