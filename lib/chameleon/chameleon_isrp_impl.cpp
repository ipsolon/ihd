/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/
#include <uhd/transport/udp_simple.hpp>
#include "chameleon_isrp_impl.hpp"
#include "chameleon_fw_common.h"

using namespace ihd;

typedef struct {
    uint32_t flags;
    uint32_t sequence;
    uint32_t addr;
    uint32_t data;
} chameleon_fw_comms_t;

chameleon_isrp_impl::chameleon_isrp_impl::chameleon_isrp_impl(uhd::device::sptr dev) :
    _seq(0), _dev(std::move(dev)), _udp_cmd_port(0)
{
    _udp_cmd_port = uhd::transport::udp_simple::make_connected(
        CHAMELEON_FW_COMMS_IPADDR,
        std::to_string(CHAMELEON_FW_COMMS_UDP_PORT));
    chameleon_fw_comms_t request = chameleon_fw_comms_t();
    request.flags =  0xDEADBEEF;
    request.sequence = _seq++;
    request.addr     = 0x731129;
    request.data     = 0x111317;

    // send request
    _udp_cmd_port->send(boost::asio::buffer(&request, sizeof(request)));
}
