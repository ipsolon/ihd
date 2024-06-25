/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "device.hpp"
#include "exception.hpp"

ihd::device::~device()
= default;

uhd::rx_streamer::sptr ihd::device::get_rx_stream(const uhd::stream_args_t& args) { THROW_NOT_IMPLEMENTED_ERROR(); }

uhd::tx_streamer::sptr ihd::device::get_tx_stream(const uhd::stream_args_t& args) { THROW_NOT_IMPLEMENTED_ERROR(); }

bool ihd::device::recv_async_msg(uhd::async_metadata_t& async_metadata, double timeout) { THROW_NOT_IMPLEMENTED_ERROR(); }
