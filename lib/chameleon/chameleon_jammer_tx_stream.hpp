//
// Created by jmeyers on 11/6/24.
//

#ifndef CHAMELEON_JAMMER_TX_STREAMER_HPP
#define CHAMELEON_JAMMER_TX_STREAMER_HPP

#include <uhd/transport/udp_simple.hpp>
#include "ipsolon_tx_stream.hpp"

namespace ihd {

class chameleon_jammer_tx_stream : public ipsolon_tx_stream {
public:
    explicit chameleon_jammer_tx_stream(const uhd::stream_args_t& stream_cmd, const uhd::device_addr_t& device_addr);
    ~chameleon_jammer_tx_stream() override;

    size_t get_num_channels(void) const override;

    size_t get_max_num_samps(void) const override;

    size_t send(const buffs_type &buffs, const size_t nsamps_per_buff, const uhd::tx_metadata_t &metadata,
                const double timeout) override;

    bool recv_async_msg(uhd::async_metadata_t &async_metadata, double timeout) override;

private:
    static constexpr uint32_t DEFAULT_JAMMER_PORT = 0x6d6a; // jm (jammer)
    static constexpr uint32_t JAMMER_PORT_TX1 = 0x6d6a;
    static constexpr uint32_t JAMMER_PORT_TX2 = JAMMER_PORT_TX1 + 1;
    static constexpr uint32_t JAMMER_PORT_TX3 = JAMMER_PORT_TX2 + 1;
    static constexpr uint32_t JAMMER_PORT_TX4 = JAMMER_PORT_TX3 + 1;
    uhd::transport::udp_simple::sptr _udp_cmd_port{};
};

}
#endif //CHAMELEON_JAMMER_TX_STREAMER_HPP
