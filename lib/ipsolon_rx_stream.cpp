/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "ipsolon_rx_stream.hpp"
#include "chameleon_rx_stream.hpp"

using namespace ihd;

const std::string ipsolon_rx_stream::stream_type::PSD_STREAM = "psd";
const std::string ipsolon_rx_stream::stream_type::STREAM_FORMAT_KEY = "stream_format";
const std::string ipsolon_rx_stream::stream_type::IQ_STREAM         = "iq";
const std::string ipsolon_rx_stream::stream_type::PACKET_SIZE_KEY   = "PACKET_SIZE";

const std::string ipsolon_rx_stream::stream_type::STREAM_DEST_IP_KEY   = "IP";
const std::string ipsolon_rx_stream::stream_type::STREAM_DEST_PORT_KEY = "PORT";

const std::string ipsolon_rx_stream::stream_type::FFT_AVG_COUNT_KEY  = "FFT_AVERAGE_COUNT";
const std::string ipsolon_rx_stream::stream_type::FFT_SIZE_KEY       = "FFT_SIZE";

ipsolon_rx_stream::sptr ipsolon_rx_stream::make(const uhd::stream_args_t& stream_cmd, const uhd::device_addr_t& device_addr)
{
    // There is only one option right now
    return std::make_shared<chameleon_rx_stream>(stream_cmd, device_addr);
}