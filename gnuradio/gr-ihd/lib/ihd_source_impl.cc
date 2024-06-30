/* -*- c++ -*- */
/*
 * Copyright 2024 Ipsolon.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ihd_source_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace ihd {

#pragma message("set the following appropriately and remove this warning")
using output_type = float;
ihd_source::sptr ihd_source::make(double center_freq)
{
    return gnuradio::make_block_sptr<ihd_source_impl>(center_freq);
}


/*
 * The private constructor
 */
ihd_source_impl::ihd_source_impl(double center_freq)
    : gr::sync_block("ihd_source",
                     gr::io_signature::make(0, 0, 0),
                     gr::io_signature::make(
                         1 /* min outputs */, 1 /*max outputs */, sizeof(output_type)))
{
}

/*
 * Our virtual destructor.
 */
ihd_source_impl::~ihd_source_impl() {}

int ihd_source_impl::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    auto out = static_cast<output_type*>(output_items[0]);

#pragma message("Implement the signal processing in your block and remove this warning")
    // Do <+signal processing+>

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace ihd */
} /* namespace gr */
