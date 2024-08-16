/* -*- c++ -*- */
/*
 * Copyright 2024 Ipsolon.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_CHAMELEON_CHAMELEON_FFT_H
#define INCLUDED_CHAMELEON_CHAMELEON_FFT_H

#include <gnuradio/chameleon/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace chameleon {

/*!
 * \brief <+description of block+>
 * \ingroup chameleon
 *
 */
class CHAMELEON_API chameleon_fft : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<chameleon_fft> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of chameleon::chameleon_fft.
     *
     * To avoid accidental use of raw pointers, chameleon::chameleon_fft's
     * constructor is in a private implementation
     * class. chameleon::chameleon_fft::make is the public interface for
     * creating new instances.
     */
    static sptr make(double center_freq, std::string& ip_addr);
};

} // namespace chameleon
} // namespace gr

#endif /* INCLUDED_CHAMELEON_CHAMELEON_FFT_H */
