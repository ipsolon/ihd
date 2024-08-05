/* -*- c++ -*- */
/*
 * Copyright 2024 Ipsolon.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_IHD_CHAMELEON_FFT_H
#define INCLUDED_IHD_CHAMELEON_FFT_H

#include <gnuradio/ihd/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace ihd {

/*!
 * \brief <+description of block+>
 * \ingroup ihd
 *
 */
class IHD_API chameleon_fft : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<chameleon_fft> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of ihd::chameleon_fft.
     *
     * To avoid accidental use of raw pointers, ihd::chameleon_fft's
     * constructor is in a private implementation
     * class. ihd::chameleon_fft::make is the public interface for
     * creating new instances.
     */
    static sptr make(double center_freq = 2300000000, std::string ip_addr = "127.0.0.1");
};

} // namespace ihd
} // namespace gr

#endif /* INCLUDED_IHD_CHAMELEON_FFT_H */
