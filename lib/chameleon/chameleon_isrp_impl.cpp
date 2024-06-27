/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/
#include <uhd/transport/udp_simple.hpp>
#include "chameleon_isrp_impl.hpp"
#include "chameleon_fw_common.h"

using namespace ihd;

chameleon_isrp_impl::chameleon_isrp_impl::chameleon_isrp_impl(uhd::device::sptr dev) :
    _seq(0), _dev(std::move(dev)), _udp_cmd_port(0)
{
    _udp_cmd_port = uhd::transport::udp_simple::make_connected(
        CHAMELEON_FW_COMMS_IPADDR,
        std::to_string(CHAMELEON_FW_COMMS_UDP_PORT));
}

uhd::device::sptr chameleon_isrp_impl::get_device()
{
    return _dev;
}

uhd::rx_streamer::sptr chameleon_isrp_impl::get_rx_stream(const uhd::stream_args_t& args)
{
    return _dev->get_rx_stream(_stream_args);
}

uhd::tx_streamer::sptr chameleon_isrp_impl::get_tx_stream(const uhd::stream_args_t& args)
{
    return _dev->get_tx_stream(_stream_args);
}

uhd::tune_result_t chameleon_isrp_impl::set_rx_freq(const uhd::tune_request_t& tune_request, size_t chan)
{
    uhd::tune_result_t tr{};
    chameleon_fw_comms_t request = chameleon_fw_comms_t();
    request.flags =  CHAMELEON_FW_COMMS_FLAGS_WRITE;
    request.sequence = _seq++;
    request.addr     = CHAMELEON_FW_COMMS_CMD_TUNE_FREQ;
    request.data     = chameleon_fw_cmd_tune_t{chan, tune_request.rf_freq};

    // send request
    _udp_cmd_port->send(boost::asio::buffer(&request, sizeof(request)));
    return tr;
}