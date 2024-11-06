//
// Created by jmeyers on 11/6/24.
//

#include "chameleon_jammer_tx_stream.hpp"

ihd::chameleon_jammer_tx_stream::chameleon_jammer_tx_stream(const uhd::stream_args_t &stream_cmd,
                                                            const uhd::device_addr_t &device_addr) {
    printf("ip addr:%s port:%d\n", device_addr["addr"].c_str(), DEFAULT_JAMMER_PORT);
    _udp_cmd_port = uhd::transport::udp_simple::make_connected(device_addr["addr"],
                                                               std::to_string(DEFAULT_JAMMER_PORT));
}

size_t ihd::chameleon_jammer_tx_stream::get_num_channels(void) const {
    return 1; // We only control one jammer channel at a time
}

size_t ihd::chameleon_jammer_tx_stream::get_max_num_samps(void) const {
    return 0;
}

size_t ihd::chameleon_jammer_tx_stream::send(const uhd::tx_streamer::buffs_type &buffs, const size_t nsamps_per_buff,
                                             const uhd::tx_metadata_t &metadata, const double timeout) {
    size_t ret = _udp_cmd_port->send(boost::asio::buffer(buffs[0], nsamps_per_buff));
    return ret;
}

bool ihd::chameleon_jammer_tx_stream::recv_async_msg(uhd::async_metadata_t &async_metadata, double timeout) {
    return false;
}

ihd::chameleon_jammer_tx_stream::~chameleon_jammer_tx_stream() = default;
