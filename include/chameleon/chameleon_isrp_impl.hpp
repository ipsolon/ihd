/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef CHAMELEON_ISRP_IMPL_HPP
#define CHAMELEON_ISRP_IMPL_HPP

#include "ipsolon_isrp.hpp"
#include "exception.hpp"
#include "chameleon_device.hpp"

namespace ihd {

class chameleon_isrp_impl : public ipsolon_isrp
{

public:
    ~chameleon_isrp_impl() override = default;

    explicit chameleon_isrp_impl(uhd::device::sptr dev);

    uhd::device::sptr      get_device() override;
    uhd::rx_streamer::sptr get_rx_stream(const uhd::stream_args_t& args) override;
    uhd::tx_streamer::sptr get_tx_stream(const uhd::stream_args_t& args) override;
    uhd::tune_result_t     set_rx_freq(const uhd::tune_request_t& tune_request, size_t chan) override;

private:
    size_t _seq;
    uhd::device::sptr _dev;
    const uhd::stream_args_t _stream_args;
    uhd::transport::udp_simple::sptr _udp_cmd_port{};
};

}
#endif //CHAMELEON_ISRP_IMPL_HPP
