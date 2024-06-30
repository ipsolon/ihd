/* -*- c++ -*- */
/*
 * Copyright 2024 Ipsolon.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_IHD_IHD_SOURCE_H
#define INCLUDED_IHD_IHD_SOURCE_H

#include <gnuradio/ihd/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace ihd {

/*!
 * \brief <+description of block+>
 * \ingroup ihd
 *
 */
class IHD_API ihd_source : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<ihd_source> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of ihd::ihd_source.
     *
     * To avoid accidental use of raw pointers, ihd::ihd_source's
     * constructor is in a private implementation
     * class. ihd::ihd_source::make is the public interface for
     * creating new instances.
     */
    static sptr make(double center_freq = 60000000000);
};

} // namespace ihd
} // namespace gr

#endif /* INCLUDED_IHD_IHD_SOURCE_H */
