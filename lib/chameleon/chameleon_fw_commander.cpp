/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

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
    size_t chameleon_fw_commander::send_request(chameleon_fw_comms &request) const
    {
        request.setSequence(_seq++);
        const char *str = request.getCommandString();
        return _udp_cmd_port->send(boost::asio::buffer(str, strlen(str)));
    }

    const char *chameleon_fw_commander::getIP() {
        return _dev_addr["addr"].c_str();
    }
} // ihd