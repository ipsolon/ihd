/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef CHAMELEON_ISRP_IMPL_HPP
#define CHAMELEON_ISRP_IMPL_HPP

#include "chameleon_fw_commander.hpp"
#include "ipsolon_isrp.hpp"
#include "chameleon_device.hpp"

namespace ihd {

class chameleon_isrp_impl : public ipsolon_isrp
{

public:
    ~chameleon_isrp_impl() override = default;

    explicit chameleon_isrp_impl(uhd::device::sptr dev, const uhd::device_addr_t& dev_addr);

    uhd::device::sptr      get_device() override;
    uhd::rx_streamer::sptr get_rx_stream(const uhd::stream_args_t& args) override;
    uhd::tx_streamer::sptr get_tx_stream(const uhd::stream_args_t& args) override;
    uhd::tune_result_t     set_rx_freq(const uhd::tune_request_t& tune_request, size_t chan) override;
    uhd::tune_result_t     set_tx_freq(const uhd::tune_request_t& tune_request, size_t chan) override;
    double                 get_rx_freq(size_t chan) override;
    double                 get_tx_freq(size_t chan) override;
    void                   set_rx_gain(double gain, const std::string& name,size_t chan) override;
    void                   set_tx_gain(double gain, const std::string& name,size_t chan) override;
    double                 get_rx_gain(const std::string& name,size_t chan) override;
    double                 get_tx_gain(const std::string& name,size_t chan) override;
    void                   set_time_now(const uhd::time_spec_t& time_spec, size_t mboard) override;
    uhd::time_spec_t       get_time_now(size_t mboard) override;

    int get_temperatures(temperature_t &temperatures, size_t timeout) override;

    int stream_stop_all() override;

private:
    uhd::tune_result_t     set_freq(const uhd::tune_request_t& tune_request, size_t chan) const;
    double                 get_freq( size_t chan)const;
    uhd::device::sptr _dev;
    chameleon_fw_commander _commander;
};

}
#endif //CHAMELEON_ISRP_IMPL_HPP
