/* -*- c++ -*- */
/*
 * Copyright 2024 Ipsolon.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "chameleon_fft_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace chameleon {

#pragma message("set the following appropriately and remove this warning")
using output_type = float;
chameleon_fft::sptr chameleon_fft::make(double center_freq, std::string& ip_addr)
{
    return gnuradio::make_block_sptr<chameleon_fft_impl>(center_freq, ip_addr);
}


/*
 * The private constructor
 */
chameleon_fft_impl::chameleon_fft_impl(double center_freq, std::string& ip_addr)
    : gr::sync_block("chameleon_fft",
                     gr::io_signature::make(0, 0, 0),
                     gr::io_signature::make(
                         1 /* min outputs */, 1 /*max outputs */, sizeof(output_type)))
{
}

/*
 * Our virtual destructor.
 */
chameleon_fft_impl::~chameleon_fft_impl() {}

int chameleon_fft_impl::work(int noutput_items,
                             gr_vector_const_void_star& input_items,
                             gr_vector_void_star& output_items)
{
    auto out = static_cast<output_type*>(output_items[0]);

#pragma message("Implement the signal processing in your block and remove this warning")
    // Do <+signal processing+>

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace chameleon */
} /* namespace gr */
