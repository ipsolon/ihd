/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef MULTI_ISRP_HPP
#define MULTI_ISRP_HPP

#include <uhd/usrp/multi_usrp.hpp>

#define THROW_NOT_IMPLEMENTED_ERROR()                \
    throw ihd::exception::runtime_error(             \
    (boost::format("function not implemented: %s\n") \
    % __FUNCTION__ ).str());

namespace ihd {

class ipsolon_isrp : public uhd::usrp::multi_usrp
{
public:
    ~ipsolon_isrp() override;
    ipsolon_isrp();
    typedef std::shared_ptr<ipsolon_isrp> sptr;
    /*!
     * Make a new multi usrp from the device address.
     * \param dev_addr the device address
     * \return a new single usrp object
     * \throws uhd::key_error no device found
     * \throws uhd::index_error fewer devices found than expected
     */
    static sptr make(const uhd::device_addr_t& dev_addr);
};

}

#endif //MULTI_ISRP_HPP
