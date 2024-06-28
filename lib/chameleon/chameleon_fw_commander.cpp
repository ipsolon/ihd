//
// Created by jmeyers on 6/28/24.
//
#include "chameleon_fw_commander.hpp"
#include "chameleon_fw_common.h"

namespace ihd {
    std::atomic<size_t> chameleon_fw_commander::_seq = 0;

    chameleon_fw_commander::chameleon_fw_commander(const uhd::device_addr_t& dev_addr)
    {
        _udp_cmd_port = uhd::transport::udp_simple::make_connected(dev_addr["addr"],
                                                                   std::to_string(CHAMELEON_FW_COMMS_UDP_PORT));
    }
    size_t chameleon_fw_commander::send_request(chameleon_fw_comms_t &request) const
    {
        request.sequence = _seq++;
        return _udp_cmd_port->send(boost::asio::buffer(&request, sizeof(request)));
    }
} // ihd