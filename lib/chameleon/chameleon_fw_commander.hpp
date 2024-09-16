/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CHAMELEON_FW_CMD_HPP
#define CHAMELEON_FW_CMD_HPP
#include <atomic>
#include <uhd/device.hpp>
#include <uhd/transport/udp_simple.hpp>
#include "chameleon_fw_common.h"

namespace ihd {

class chameleon_fw_commander {
public:
    explicit chameleon_fw_commander(uhd::device_addr_t  dev_addr);
    int send_request(chameleon_fw_comms &request, int timeout_ms = 0) const;
    const char *getIP();
private:
    uhd::transport::udp_simple::sptr _udp_cmd_port{};
    static std::atomic<size_t> _seq;
    uhd::device_addr_t _dev_addr;
};

} // ihd

#endif //CHAMELEON_FW_CMD_HPP
