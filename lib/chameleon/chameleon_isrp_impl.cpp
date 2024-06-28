/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "chameleon_isrp_impl.hpp"
#include "chameleon_fw_common.h"

using namespace ihd;

chameleon_isrp_impl::chameleon_isrp_impl(uhd::device::sptr dev, const uhd::device_addr_t& dev_addr) : _dev(std::move(dev)), _commander(dev_addr) {}

uhd::device::sptr chameleon_isrp_impl::get_device()
{
    return _dev;
}

uhd::rx_streamer::sptr chameleon_isrp_impl::get_rx_stream(const uhd::stream_args_t& args)
{
    return _dev->get_rx_stream(args);
}

uhd::tx_streamer::sptr chameleon_isrp_impl::get_tx_stream(const uhd::stream_args_t& args)
{
    return _dev->get_tx_stream(args);
}

uhd::tune_result_t chameleon_isrp_impl::set_rx_freq(const uhd::tune_request_t& tune_request, size_t chan)
{
    uhd::tune_result_t tr{};
    chameleon_fw_comms_t request = chameleon_fw_comms_t();
    request.flags =  CHAMELEON_FW_COMMS_FLAGS_WRITE;
    request.addr     = CHAMELEON_FW_COMMS_CMD_TUNE_FREQ;
    request.tune     = chameleon_fw_cmd_tune_t{chan, tune_request.rf_freq};

    // send request
    _commander.send_request(request);
    return tr;
}