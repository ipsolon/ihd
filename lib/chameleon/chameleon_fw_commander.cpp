//
// Created by jmeyers on 6/28/24.
//
#include "chameleon_fw_commander.hpp"

#include <utility>
#include "chameleon_fw_common.h"

namespace ihd {
    std::atomic<size_t> chameleon_fw_commander::_seq = 0;

    chameleon_fw_commander::chameleon_fw_commander(uhd::device_addr_t  da) : _dev_addr(std::move(da))
    {
        _udp_cmd_port = uhd::transport::udp_simple::make_connected(_dev_addr["addr"],
                                                                   std::to_string(CHAMELEON_FW_COMMS_UDP_PORT));
    }
    size_t chameleon_fw_commander::send_request(chameleon_fw_comms_t &request) const
    {
        request.sequence = _seq++;
        return _udp_cmd_port->send(boost::asio::buffer(&request, sizeof(request)));
    }

    const char *chameleon_fw_commander::getIP() {
        return _dev_addr["add"].c_str();
    }
} // ihd