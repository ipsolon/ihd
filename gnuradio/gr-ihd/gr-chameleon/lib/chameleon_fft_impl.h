/* -*- c++ -*- */
/*
 * Copyright 2024 Ipsolon.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_CHAMELEON_CHAMELEON_FFT_IMPL_H
#define INCLUDED_CHAMELEON_CHAMELEON_FFT_IMPL_H

#include <gnuradio/chameleon/chameleon_fft.h>

namespace gr {
namespace chameleon {

class chameleon_fft_impl : public chameleon_fft
{
private:
    double _center_freq;
    std::string _ipaddr;

public:
    chameleon_fft_impl(double center_freq, std::string ip_addr);
    ~chameleon_fft_impl() override;

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace chameleon
} // namespace gr

#endif /* INCLUDED_CHAMELEON_CHAMELEON_FFT_IMPL_H */
