/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "multi_isrp.hpp"
#include "exception.hpp"

#define THROW_NOT_IMPLEMENTED_ERROR()                \
    throw ihd::exception::runtime_error(             \
    (boost::format("function not implemented: %s\n") \
    % __FUNCTION__ ).str());

using namespace ihd;

multi_isrp::~multi_isrp()
{
}

multi_isrp::multi_isrp()
{
}

class multi_isrp_impl : public multi_isrp
{

public:
    multi_isrp_impl(uhd::device::sptr dev) : multi_isrp(), _dev(dev) {}
    ~multi_isrp_impl() {};

    uhd::device::sptr get_device()
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }

    uhd::property_tree::sptr get_tree() const
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }

    uhd::rx_streamer::sptr get_rx_stream(const uhd::stream_args_t& args)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::tx_streamer::sptr get_tx_stream(const uhd::stream_args_t& args)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::dict<std::string, std::string> get_usrp_rx_info(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::dict<std::string, std::string> get_usrp_tx_info(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_master_clock_rate(double rate, size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    double get_master_clock_rate(size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::meta_range_t get_master_clock_rate_range(const size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::string get_pp_string()
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::string get_mboard_name(size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::time_spec_t get_time_now(size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::time_spec_t get_time_last_pps(size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_time_now(const uhd::time_spec_t& time_spec, size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_time_next_pps(const uhd::time_spec_t& time_spec, size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_time_unknown_pps(const uhd::time_spec_t& time_spec)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    bool get_time_synchronized()
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_command_time(const uhd::time_spec_t& time_spec, size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void clear_command_time(size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void issue_stream_cmd(const uhd::stream_cmd_t& stream_cmd, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_time_source(const std::string& source, const size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::string get_time_source(const size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::vector<std::string> get_time_sources(const size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_clock_source(const std::string& source, const size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::string get_clock_source(const size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::vector<std::string> get_clock_sources(const size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_sync_source(const std::string& clock_source, const std::string& time_source,
        const size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_sync_source(const uhd::device_addr_t& sync_source, const size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::device_addr_t get_sync_source(const size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::vector<uhd::device_addr_t> get_sync_sources(const size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_clock_source_out(const bool enb, const size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_time_source_out(const bool enb, const size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    size_t get_num_mboards()
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::sensor_value_t get_mboard_sensor(const std::string& name, size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::vector<std::string> get_mboard_sensor_names(size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_user_register(const uint8_t addr, const uint32_t data, size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::wb_iface::sptr get_user_settings_iface(const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::rfnoc::radio_control& get_radio_control(const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_rx_subdev_spec(const uhd::usrp::subdev_spec_t& spec, size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::usrp::subdev_spec_t get_rx_subdev_spec(size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    size_t get_rx_num_channels()
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::string get_rx_subdev_name(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_rx_rate(double rate, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_rx_spp(const size_t spp, const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    double get_rx_rate(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::meta_range_t get_rx_rates(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::tune_result_t set_rx_freq(const uhd::tune_request_t& tune_request, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    double get_rx_freq(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::freq_range_t get_rx_freq_range(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::freq_range_t get_fe_rx_freq_range(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::vector<std::string> get_rx_lo_names(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_rx_lo_source(const std::string& src, const std::string& name, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    const std::string get_rx_lo_source(const std::string& name, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::vector<std::string> get_rx_lo_sources(const std::string& name, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_rx_lo_export_enabled(bool enabled, const std::string& name, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    bool get_rx_lo_export_enabled(const std::string& name, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    double set_rx_lo_freq(double freq, const std::string& name, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    double get_rx_lo_freq(const std::string& name, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::freq_range_t get_rx_lo_freq_range(const std::string& name, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::vector<std::string> get_tx_lo_names(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_tx_lo_source(const std::string& src, const std::string& name, const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    const std::string get_tx_lo_source(const std::string& name, const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::vector<std::string> get_tx_lo_sources(const std::string& name, const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_tx_lo_export_enabled(const bool enabled, const std::string& name, const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    bool get_tx_lo_export_enabled(const std::string& name, const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    double set_tx_lo_freq(const double freq, const std::string& name, const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    double get_tx_lo_freq(const std::string& name, const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::freq_range_t get_tx_lo_freq_range(const std::string& name, const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_rx_gain(double gain, const std::string& name, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::vector<std::string> get_rx_gain_profile_names(const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_rx_gain_profile(const std::string& profile, const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::string get_rx_gain_profile(const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_normalized_rx_gain(double gain, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_rx_agc(bool enable, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    double get_rx_gain(const std::string& name, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    double get_normalized_rx_gain(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::gain_range_t get_rx_gain_range(const std::string& name, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::vector<std::string> get_rx_gain_names(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_rx_antenna(const std::string& ant, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::string get_rx_antenna(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::vector<std::string> get_rx_antennas(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_rx_bandwidth(double bandwidth, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    double get_rx_bandwidth(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::meta_range_t get_rx_bandwidth_range(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::usrp::dboard_iface::sptr get_rx_dboard_iface(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::sensor_value_t get_rx_sensor(const std::string& name, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::vector<std::string> get_rx_sensor_names(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_rx_dc_offset(const bool enb, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_rx_dc_offset(const std::complex<double>& offset, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::meta_range_t get_rx_dc_offset_range(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_rx_iq_balance(const bool enb, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_rx_iq_balance(const std::complex<double>& correction, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    bool has_rx_power_reference(const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_rx_power_reference(const double power_dbm, const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    double get_rx_power_reference(const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::meta_range_t get_rx_power_range(const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_tx_subdev_spec(const uhd::usrp::subdev_spec_t& spec, size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::usrp::subdev_spec_t get_tx_subdev_spec(size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    size_t get_tx_num_channels()
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::string get_tx_subdev_name(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_tx_rate(double rate, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    double get_tx_rate(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::meta_range_t get_tx_rates(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::tune_result_t set_tx_freq(const uhd::tune_request_t& tune_request, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    double get_tx_freq(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::freq_range_t get_tx_freq_range(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::freq_range_t get_fe_tx_freq_range(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_tx_gain(double gain, const std::string& name, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::vector<std::string> get_tx_gain_profile_names(const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_tx_gain_profile(const std::string& profile, const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::string get_tx_gain_profile(const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_normalized_tx_gain(double gain, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    double get_tx_gain(const std::string& name, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    double get_normalized_tx_gain(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::gain_range_t get_tx_gain_range(const std::string& name, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::vector<std::string> get_tx_gain_names(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    bool has_tx_power_reference(const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_tx_power_reference(const double power_dbm, const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    double get_tx_power_reference(const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::meta_range_t get_tx_power_range(const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_tx_antenna(const std::string& ant, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::string get_tx_antenna(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::vector<std::string> get_tx_antennas(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_tx_bandwidth(double bandwidth, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    double get_tx_bandwidth(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::meta_range_t get_tx_bandwidth_range(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::usrp::dboard_iface::sptr get_tx_dboard_iface(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::sensor_value_t get_tx_sensor(const std::string& name, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::vector<std::string> get_tx_sensor_names(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_tx_dc_offset(const std::complex<double>& offset, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::meta_range_t get_tx_dc_offset_range(size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_tx_iq_balance(const std::complex<double>& correction, size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::vector<std::string> get_gpio_banks(const size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_gpio_attr(const std::string& bank, const std::string& attr, const uint32_t value,
        const uint32_t mask, const size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uint32_t get_gpio_attr(const std::string& bank, const std::string& attr, const size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::vector<std::string> get_gpio_src_banks(const size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::vector<std::string> get_gpio_srcs(const std::string& bank, const size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::vector<std::string> get_gpio_src(const std::string& bank, const size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_gpio_src(const std::string& bank, const std::vector<std::string>& src, const size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::vector<std::string> get_rx_filter_names(const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::filter_info_base::sptr get_rx_filter(const std::string& name, const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_rx_filter(const std::string& name, uhd::filter_info_base::sptr filter, const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    std::vector<std::string> get_tx_filter_names(const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::filter_info_base::sptr get_tx_filter(const std::string& name, const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    void set_tx_filter(const std::string& name, uhd::filter_info_base::sptr filter, const size_t chan)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }


    uhd::rfnoc::mb_controller& get_mb_controller(const size_t mboard)
    {
        THROW_NOT_IMPLEMENTED_ERROR();
    }

private:
    uhd::device::sptr _dev;
};

/***********************************************************************
 * The Make Function
 **********************************************************************/

multi_isrp::sptr multi_isrp::make(const uhd::device_addr_t& dev_addr)
{
    UHD_LOGGER_TRACE("MULTI_USRP")
        << "multi_usrp::make with args " << dev_addr.to_pp_string();

    uhd::device::sptr dev = nullptr;


    return std::make_shared<multi_isrp_impl>(dev);
}
