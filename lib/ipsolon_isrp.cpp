/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/
#include <uhd/transport/udp_simple.hpp>

#include "ipsolon_isrp.hpp"
#include "chameleon_device.hpp"
#include "chameleon_isrp_impl.hpp"

using namespace ihd;

ipsolon_isrp::~ipsolon_isrp()
{
}

ipsolon_isrp::ipsolon_isrp()
{
}

/***********************************************************************
 * The Make Function
 **********************************************************************/

ipsolon_isrp::sptr ipsolon_isrp::make(const uhd::device_addr_t& dev_addr)
{
    UHD_LOGGER_TRACE("MULTI_USRP")
        << "multi_usrp::make with args " << dev_addr.to_pp_string();

    // For now, there is only one type
    uhd::device::sptr dev = chameleon_device::chameleon_make(dev_addr);

    return std::make_shared<chameleon_isrp_impl>(dev, dev_addr);
}
