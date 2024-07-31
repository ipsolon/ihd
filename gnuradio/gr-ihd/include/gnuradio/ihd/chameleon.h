/* -*- c++ -*- */
/*
 * Copyright 2024 Ipsolon.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_IHD_CHAMELEON_H
#define INCLUDED_IHD_CHAMELEON_H

#include <gnuradio/ihd/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace ihd {

/*!
 * \brief <+description of block+>
 * \ingroup ihd
 *
 */
class IHD_API chameleon : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<chameleon> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of ihd::chameleon.
     *
     * To avoid accidental use of raw pointers, ihd::chameleon's
     * constructor is in a private implementation
     * class. ihd::chameleon::make is the public interface for
     * creating new instances.
     */
    static sptr make(double center_freq = 6000000.0, std::string ip_addr = "127.0.0.1");
};

} // namespace ihd
} // namespace gr

#endif /* INCLUDED_IHD_CHAMELEON_H */
