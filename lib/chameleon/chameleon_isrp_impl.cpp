/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/
#include <boost/algorithm/string/trim.hpp>
#include "chameleon_isrp_impl.hpp"
#include "chameleon_fw_common.hpp"
#include "chameleon_jammer_block_ctrl.hpp"
#include "debug.hpp"

using namespace ihd;

chameleon_isrp_impl::chameleon_isrp_impl(uhd::device::sptr dev,
                                         const uhd::device_addr_t &dev_addr) : _dev(std::move(dev)),
                                                                               _commander(dev_addr) {
}

uhd::device::sptr chameleon_isrp_impl::get_device() {
    return _dev;
}

uhd::rx_streamer::sptr chameleon_isrp_impl::get_rx_stream(const uhd::stream_args_t &args) {
    return _dev->get_rx_stream(args);
}

uhd::tx_streamer::sptr chameleon_isrp_impl::get_tx_stream(const uhd::stream_args_t &args) {
    return _dev->get_tx_stream(args);
}

uhd::tune_result_t chameleon_isrp_impl::set_freq(const uhd::tune_request_t &tune_request, size_t chan) const {
    constexpr size_t rx_set_freq_timeout_ms = 5000;
    constexpr uint32_t internal_path_delay_cal = 0x200;
    constexpr uint32_t loopback_lo_delay_cal = 0x2000;
    constexpr uint32_t qec_cal = 0x10000;
    constexpr uint32_t default_cal = qec_cal | loopback_lo_delay_cal | internal_path_delay_cal;
    uint32_t cal_mask = default_cal;

    if (tune_request.args.has_key("calmask")) {
        cal_mask = stoi(tune_request.args["calmask"], nullptr, 16);
    }
    if (tune_request.args.has_key("qec_cal")) {
        auto do_qec = tune_request.args["qec_cal"];
        if (do_qec == "true") {
            cal_mask |= qec_cal;
        } else if (do_qec == "false") {
            cal_mask &= ~qec_cal;
        }
    }
    uhd::tune_result_t tr{};
    std::unique_ptr<chameleon_fw_cmd> tune_cmd(
            new chameleon_fw_cmd_tune(chan, static_cast<uint64_t>(tune_request.rf_freq), cal_mask));

    chameleon_fw_comms request(std::move(tune_cmd));

    // send request
    _commander.send_request(request, rx_set_freq_timeout_ms);

    dbprintf("set_freq response: \n");
    for (const auto &token: request.getResponse()) {
        dbprintf("%s\n", token.c_str());
    }
    // TODO implement tune result
    return tr;
}

double chameleon_isrp_impl::get_freq(size_t chan) const {
    constexpr size_t rx_get_freq_timeout_ms = 5000;
    double ret = -1;

    std::unique_ptr<chameleon_fw_cmd> tune_cmd(
            new chameleon_fw_cmd_tune_get(chan));

    chameleon_fw_comms request(std::move(tune_cmd));

    // send request
    _commander.send_request(request, rx_get_freq_timeout_ms);

    auto result = request.getResult();
    if (result == chameleon_fw_comms::ACK) {
        std::string const x = request.getResponse().at(2);
        ret = std::stod(x.substr(x.find('=') + 1));
    }
    return ret;
}

uhd::tune_result_t chameleon_isrp_impl::set_rx_freq(const uhd::tune_request_t &tune_request, size_t chan) {
    return set_freq(tune_request, chan);
}

uhd::tune_result_t chameleon_isrp_impl::set_tx_freq(const uhd::tune_request_t &tune_request, size_t chan) {
    return set_freq(tune_request, chan);
}

double chameleon_isrp_impl::get_rx_freq(size_t chan) {
    return get_freq(chan);
}

double chameleon_isrp_impl::get_tx_freq(size_t chan) {
    return get_freq(chan);
}

void chameleon_isrp_impl::set_rx_gain(double gain, const std::string &name, size_t chan) {
    constexpr size_t rx_set_gain_timeout_ms = 5000;
    std::unique_ptr<chameleon_fw_cmd> gain_cmd(new chameleon_fw_cmd_rxgain(chan, gain));

    chameleon_fw_comms request(std::move(gain_cmd));

    // send request
    _commander.send_request(request, rx_set_gain_timeout_ms);
}

double chameleon_isrp_impl::get_rx_gain(const std::string &name, size_t chan) {
    int err = 0;
    double ret = -1;
    constexpr size_t rx_get_gain_timeout_ms = 5000;
    std::unique_ptr<chameleon_fw_cmd> gain_cmd(new chameleon_fw_cmd_get_rxgain(chan));

    chameleon_fw_comms request(std::move(gain_cmd));

    // send request
    _commander.send_request(request, rx_get_gain_timeout_ms);
    auto result = request.getResult();
    if (result == chameleon_fw_comms::ACK) {
        std::string const x = request.getResponse().at(2);
        ret = std::stod(x.substr(x.find('=') + 1));
    }
    return ret;
}

void chameleon_isrp_impl::set_tx_gain(double gain, const std::string &name, size_t chan) {
    constexpr int tx_set_gain_timeout_ms = 5000;
    std::unique_ptr<chameleon_fw_cmd> gain_cmd(new chameleon_fw_cmd_txgain(chan, gain));

    chameleon_fw_comms request(std::move(gain_cmd));

    // send request
    _commander.send_request(request, tx_set_gain_timeout_ms);
}

double chameleon_isrp_impl::get_tx_gain(const std::string &name, size_t chan) {
    constexpr int tx_get_gain_timeout_ms = 5000;
    double ret = -1;
    std::unique_ptr<chameleon_fw_cmd> gain_cmd(new chameleon_fw_cmd_get_txgain(chan));

    chameleon_fw_comms request(std::move(gain_cmd));

    // send request
    _commander.send_request(request, tx_get_gain_timeout_ms);
    auto result = request.getResult();
    if (result == chameleon_fw_comms::ACK) {
        std::string const x = request.getResponse().at(2);
        ret = std::stod(x.substr(x.find('=') + 1));
    }
    return ret;
}

uhd::time_spec_t chameleon_isrp_impl::get_time_now(size_t mboard) {
    constexpr int timeout_ms = 5000;

    uhd::time_spec_t ts{};

    std::unique_ptr<chameleon_fw_cmd> get_time_cmd(new chameleon_fw_get_time());
    chameleon_fw_comms request(std::move(get_time_cmd));
    _commander.send_request(request, timeout_ms);

    auto result = request.getResult();
    if (result == chameleon_fw_comms::ACK) {
        const double seconds = 0.0;
        auto resp = request.getResponse();
        if (resp.size() >= 3) {
            const auto& time_str = resp[2];
            auto ret = std::stod(time_str.substr(time_str.find('=') + 1));
            ts = uhd::time_spec_t(ret);
        }
    }
    return ts;
}

void chameleon_isrp_impl::set_time_now(const uhd::time_spec_t &time_spec, size_t mboard) {
    constexpr int timeout_ms = 5000;
    std::unique_ptr<chameleon_fw_cmd> set_time_cmd(new chameleon_fw_set_time(time_spec.get_full_secs()));

    chameleon_fw_comms request(std::move(set_time_cmd));

    _commander.send_request(request, timeout_ms);
    auto result = request.getResult();
    if (result != chameleon_fw_comms::ACK) {
        dbprintf("Request failed: %u", result);
    }
}

int chameleon_isrp_impl::get_temperatures(temperature_t &temperatures, size_t timeout) {
    std::unique_ptr<chameleon_fw_cmd> temp_cmd(new chameleon_fw_get_temps_all);
    chameleon_fw_comms request(std::move(temp_cmd));

    auto err = _commander.send_request(request, timeout);
    if (!err) {
        for (const auto &token: request.getResponse()) {
            std::string msg(token);
            boost::algorithm::trim(msg);
            dbprintf("%s\n", msg.c_str());

            std::stringstream tokenStream(msg);
            std::string key, value;
            const char seperator = '=';
            if (std::getline(tokenStream, key, seperator) && std::getline(tokenStream, value)) {
                double double_value = 0.0;
                try {
                    double_value = std::stod(value);
                    dbprintf("Converted double: %f\n", double_value);
                } catch (const std::invalid_argument &e) {
                    dbprintf("Invalid argument: %s\n", e.what());
                    err = -1;
                } catch (const std::out_of_range &e) {
                    dbprintf("Out of range: %s\n", e.what());
                    err = -1;
                }
                temperatures[key] = double_value;
            }
        }
    }
    return err;
}
