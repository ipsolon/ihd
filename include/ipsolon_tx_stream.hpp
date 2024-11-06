//
// Created by jmeyers on 10/15/24.
//

#ifndef EXAMPLE_IPSOLON_TX_STREAM_HPP
#define EXAMPLE_IPSOLON_TX_STREAM_HPP
#include <uhd/stream.hpp>

namespace ihd {

    class ipsolon_tx_stream : public uhd::tx_streamer {
    public:
        typedef std::shared_ptr<ipsolon_tx_stream> sptr;
        static sptr make(const uhd::stream_args_t& stream_cmd, const uhd::device_addr_t& _device_addr);
    };
}
#endif //EXAMPLE_IPSOLON_TX_STREAM_HPP
