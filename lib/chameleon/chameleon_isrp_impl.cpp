/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "chameleon_isrp_impl.hpp"
#include "chameleon_fw_common.hpp"

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
    const int rx_set_freq_timeout_ms = 5000;
    uhd::tune_result_t tr{};
    std::unique_ptr<chameleon_fw_cmd> tune_cmd(
            new chameleon_fw_cmd_tune(chan, static_cast<uint64_t>(tune_request.rf_freq)));

    chameleon_fw_comms request(std::move(tune_cmd));

    // send request
    _commander.send_request(request, rx_set_freq_timeout_ms);
    // TODO implement tune result
    return tr;
}

void chameleon_isrp_impl::set_rx_gain(double gain, const std::string& name, size_t chan)
{
    THROW_NOT_IMPLEMENTED_ERROR();
}
