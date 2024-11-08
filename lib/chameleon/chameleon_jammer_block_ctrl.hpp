//
// Created by jmeyers on 11/5/24.
//

#ifndef CHAMELEON_JAMMER_BLOCK_CTRL_HPP
#define CHAMELEON_JAMMER_BLOCK_CTRL_HPP

#include <cstdlib>
#include <cstdint>
#include <ccomplex>
#include <map>
#include <vector>
#include <uhd/device.hpp>

#include "ipsolon_tx_stream.hpp"
#include "ipsolon_block_ctrl.hpp"

namespace ihd {

    typedef enum {
        BANK_A = 0,
        BANK_B = 1
    } jammer_bank_t;

    class jammer_config_t {
    public:
        jammer_bank_t bank;
        uint32_t dwell;
        float fm_max_dev;
        float fm_ddang;
        std::map<uint32_t, std::complex<float>> phasors;
        std::vector<float> centers;

        jammer_config_t() :
                bank(BANK_A),
                dwell(1),
                fm_max_dev(0.0f),
                fm_ddang(0.0f) {
        }
    };

    class chameleon_jammer_block_ctrl : virtual public ipsolon_block_ctrl {
    public:
        static const uint32_t ID;
        // Command Magic Words
        static const uint32_t CMD_CONFIG_A = 0x00aaaa00;
        static const uint32_t CMD_CONFIG_B = 0x00bbbb00;
        static const uint32_t CMD_START_A = 0x1111aaaa;
        static const uint32_t CMD_START_B = 0x1111bbbb;
        static const uint32_t CMD_STOP = 0xffffffff;

        // Methods

        explicit chameleon_jammer_block_ctrl();

        virtual ~chameleon_jammer_block_ctrl() = default;

        void set_streamer(uhd::tx_streamer::sptr stream);

        void send_config(jammer_config_t config);

        void start(jammer_bank_t bank, uhd::time_spec_t time);

        void stop();

    private:
        void convert_start(jammer_bank_t bank, std::vector<uint32_t> &y);

        void convert_stop(std::vector<uint32_t> &y);

        void convert_config(jammer_config_t &config, std::vector<uint32_t> &y);

        uhd::tx_streamer::sptr stream;
        std::vector<uint32_t> payload;
        uhd::tx_metadata_t md;
    };

}
#endif //CHAMELEON_JAMMER_BLOCK_CTRL_HPP
