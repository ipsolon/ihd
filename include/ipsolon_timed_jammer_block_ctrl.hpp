#ifndef INCLUDED_IPSOLON_TIMED_JAMMER_BLOCK_CTRL_HPP
#define INCLUDED_IPSOLON_TIMED_JAMMER_BLOCK_CTRL_HPP
#include <stdint.h>
#include <map>
#include <complex>
#include <vector>

#include <uhd.h>
#include <uhd/stream.hpp>

namespace ihd {

typedef enum {
        BANK_A = 0,
        BANK_B = 1
} jammer_bank_t ;

class jammer_config_t {
    public:
        jammer_bank_t                           bank ;
        uint32_t                                dwell ;
        float                                   fm_max_dev ;
        float                                   fm_ddang ;
        std::map<uint32_t, std::complex<float>> phasors ;
        std::vector<float>                      centers ;

        jammer_config_t() :
            bank(BANK_A),
            dwell(1),
            fm_max_dev(0.0f),
            fm_ddang(0.0f)
        {
        }
} ;

/*! \brief Block controller for the Citadel Timed Jammer block.
 *
 * This packet creates a burst at a specific frequency offset,
 * with a max deviation, over duration, incrementing by ddang.
 */
class timed_jammer_block_ctrl
{
public:
    // Settings Registers
    static const uint32_t SR_ENABLE             = 130 ;
    static const uint32_t SR_PAYLOAD_LENGTH     = 131 ;

    static const uint32_t SR_BANK_SELECT        = 132 ;
    static const uint32_t SR_FM_DEV             = 133 ;
    static const uint32_t SR_FM_DDANG           = 134 ;

    static const uint32_t SR_PHASORS_IDX_ADDR_A = 135 ;
    static const uint32_t SR_PHASORS_ADDR_A     = 136 ;
    static const uint32_t SR_CENTERS_IDX_ADDR_A = 137 ;
    static const uint32_t SR_CENTERS_ADDR_A     = 138 ;
    static const uint32_t SR_DWELL_COUNT_A      = 139 ;
    static const uint32_t SR_CENTER_COUNT_A     = 140 ;

    static const uint32_t SR_PHASORS_IDX_ADDR_B = 141 ;
    static const uint32_t SR_PHASORS_ADDR_B     = 142 ;
    static const uint32_t SR_CENTERS_IDX_ADDR_B = 143 ;
    static const uint32_t SR_CENTERS_ADDR_B     = 144 ;
    static const uint32_t SR_DWELL_COUNT_B      = 145 ;
    static const uint32_t SR_CENTER_COUNT_B     = 146 ;

    static const uint32_t SR_OVERFLOW_CLEAR     = 147 ;
    static const uint32_t SR_RESET              = 148 ;

    // Readback Registers
    static const uint32_t RB_OVERFLOW           = 0 ;

    // Command Magic Words
    static const uint32_t CMD_CONFIG_A          = 0x00aaaa00 ;
    static const uint32_t CMD_CONFIG_B          = 0x00bbbb00 ;
    static const uint32_t CMD_START_A           = 0x1111aaaa ;
    static const uint32_t CMD_START_B           = 0x1111bbbb ;
    static const uint32_t CMD_STOP              = 0xffffffff ;

    // Methods
    virtual bool overflow() = 0 ;
    virtual void clear_overflow() = 0 ;
    virtual void set_streamer(uhd::tx_streamer::sptr stream) = 0 ;
    virtual void send_config(jammer_config_t config) = 0 ;
    virtual void start(jammer_bank_t bank, uhd::time_spec_t time) = 0 ;
    virtual void stop() = 0 ;

}; /* class timed_jammer_block_ctrl*/


} /* namespace ihd */

#endif
