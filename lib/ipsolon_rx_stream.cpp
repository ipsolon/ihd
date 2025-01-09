/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "ipsolon_rx_stream.hpp"
#include "chameleon_rx_stream_iq.hpp"
#include "chameleon_rx_stream_psd.hpp"
#include "ihd.h"

using namespace ihd;

const std::string ipsolon_rx_stream::stream_type::PSD_STREAM = "psd";
const std::string ipsolon_rx_stream::stream_type::STREAM_FORMAT_KEY = "stream_format";
const std::string ipsolon_rx_stream::stream_type::IQ_STREAM = "iq";

const std::string ipsolon_rx_stream::stream_type::STREAM_DEST_IP_KEY = "IP";
const std::string ipsolon_rx_stream::stream_type::STREAM_DEST_PORT_KEY = "PORT";

const std::string ipsolon_rx_stream::stream_type::FFT_AVG_COUNT_KEY = "FFT_AVERAGE_COUNT";
const std::string ipsolon_rx_stream::stream_type::FFT_SIZE_KEY = "FFT_SIZE";

ipsolon_rx_stream::sptr ipsolon_rx_stream::make(const uhd::stream_args_t &stream_cmd,
                                                const uhd::device_addr_t &device_addr) {
    // There is only one option right now
    std::string type_str = stream_cmd.args[ipsolon_rx_stream::stream_type::STREAM_FORMAT_KEY];
    stream_type st(type_str);
    dbprintf("ipsolon_rx_stream::make stream_type=%s\n",type_str.c_str());
    if (st.modeEquals(stream_type::PSD_STREAM)) {
        return std::make_shared<chameleon_rx_stream_psd>(stream_cmd, device_addr);
    } else {
        return std::make_shared<chameleon_rx_stream_iq>(stream_cmd, device_addr);
    }
}
