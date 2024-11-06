//
// Created by jmeyers on 10/15/24.
//

#include "ipsolon_tx_stream.hpp"
#include "chameleon_jammer_tx_stream.hpp"

using namespace ihd;

ipsolon_tx_stream::sptr ipsolon_tx_stream::make(const uhd::stream_args_t &stream_cmd, const uhd::device_addr_t &device_addr) {
    // There is only one option right now
    return std::make_shared<chameleon_jammer_tx_stream>(stream_cmd, device_addr);
}

