//
// Created by jmeyers on 11/5/24.
//
#include "chameleon_jammer_block_ctrl.hpp"

#include <utility>

using namespace ihd;

const uint32_t chameleon_jammer_block_ctrl::ID = 0;

chameleon_jammer_block_ctrl::chameleon_jammer_block_ctrl()  : ipsolon_block_ctrl (chameleon_jammer_block_ctrl::ID) {
    md.start_of_burst = true;
    md.end_of_burst = true;
    stream = nullptr;
}

void chameleon_jammer_block_ctrl::convert_start(jammer_bank_t bank, std::vector<uint32_t> &y) {
    y.resize(1);
    y[0] = (bank == BANK_A) ? CMD_START_A : CMD_START_B;
}

void chameleon_jammer_block_ctrl::convert_stop(std::vector<uint32_t> &y) {
    y.resize(1);
    y[0] = CMD_STOP;
}

void chameleon_jammer_block_ctrl::convert_config(jammer_config_t &config, std::vector<uint32_t> &y) {
    static constexpr uint32_t FIXED_SIZES = 7;
    y.resize(FIXED_SIZES + 2 * config.phasors.size() + config.centers.size());
    y[0] = (config.bank == BANK_A) ? CMD_CONFIG_A : CMD_CONFIG_B;
    y[1] = config.dwell;
    y[2] = static_cast<uint32_t>(config.fm_max_dev * powf(2, 32) / (2.0f * M_PI));
    y[3] = static_cast<uint32_t>(config.fm_ddang * powf(2, 32) / (2.0f * M_PI));
    y[4] = config.phasors.size();
    y[5] = 0;
    uint32_t idx = 6;
    for (auto phasor: config.phasors) {
        y[idx++] = phasor.first;
        auto val = static_cast<uint32_t>((phasor.second.real() * 28126.0f));
        val <<= 16;
        val |= (static_cast<uint32_t>(phasor.second.imag() * 28126.0f) & 0xffff);
        y[idx++] = val;
    }
    y[idx++] = config.centers.size();
    for (auto center: config.centers) {
        y[idx++] = static_cast<uint32_t>(center / (2.0f * M_PI) * (powf(2, 32.0f) - 1));
    }
}

// Give the streamer to the object to be able to use it
void chameleon_jammer_block_ctrl::set_streamer(uhd::tx_streamer::sptr stream) {
    this->stream = std::move(stream);
}

// Send the config down to the device
void chameleon_jammer_block_ctrl::send_config(jammer_config_t config) {
    convert_config(config, payload);
    md.has_time_spec = false;
    stream->send(&payload.front(), payload.size(), md, 5.0);
}

int chameleon_jammer_block_ctrl::start(jammer_bank_t bank, uhd::time_spec_t time) {
    int err = 0;
    convert_start(bank, payload);
    md.has_time_spec = true;
    md.time_spec = time;
    size_t size_sent = stream->send(&payload.front(), payload.size(), md, 5.0);
    err = (size_sent != payload.size());
    return err;
}

void chameleon_jammer_block_ctrl::stop() {
    convert_stop(payload);
    md.has_time_spec = false;
    stream->send(&payload.front(), payload.size(), md, 5.0);
}
