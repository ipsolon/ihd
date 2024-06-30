/* -*- c++ -*- */
/*
 * Copyright 2024 Ipsolon.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_IHD_IHD_SOURCE_IMPL_H
#define INCLUDED_IHD_IHD_SOURCE_IMPL_H

#include <gnuradio/ihd/ihd_source.h>

namespace gr {
namespace ihd {

class ihd_source_impl : public ihd_source
{
private:
    // Nothing to declare in this block.

public:
    ihd_source_impl(double center_freq);
    ~ihd_source_impl();

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace ihd
} // namespace gr

#endif /* INCLUDED_IHD_IHD_SOURCE_IMPL_H */
