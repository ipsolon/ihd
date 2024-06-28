/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef DEVICE_HPP
#define DEVICE_HPP
#include <uhd/device.hpp>

namespace ihd {

class chameleon_device : public uhd::device
{
public:
    explicit chameleon_device(const uhd::device_addr_t& _device_addr);
    ~chameleon_device() override;
    uhd::rx_streamer::sptr get_rx_stream(const uhd::stream_args_t& args) override;
    uhd::tx_streamer::sptr get_tx_stream(const uhd::stream_args_t& args) override;
    bool recv_async_msg(uhd::async_metadata_t& async_metadata, double timeout) override;

    static uhd::device::sptr chameleon_make(const uhd::device_addr_t& device_addr);

private:
    uhd::device_addr_t _device_addr;
};

}
#endif //DEVICE_HPP
