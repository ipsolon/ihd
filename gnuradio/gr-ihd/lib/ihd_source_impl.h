/* -*- c++ -*- */
/*
 * Copyright 2024 Ipsolon.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_IHD_IHD_SOURCE_IMPL_H
#define INCLUDED_IHD_IHD_SOURCE_IMPL_H

#include <gnuradio/ihd/ihd_source.h>
#include "ihd.h"

namespace gr {
namespace ihd {

class ihd_source_impl : public ihd_source
{
private:
    // Nothing to declare in this block.

public:
    explicit ihd_source_impl(double center_freq);
    ~ihd_source_impl() override;

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

    bool stop() override;
    bool start() override;

private:
    double _center_freq;
    bool _issue_stream_cmd_on_start;
    //! Stream args
    uhd::stream_args_t _stream_args;
    //! Reference to the actual streamer
    uhd::rx_streamer::sptr _streamer;
    //! True if d_start_time holds a value we need to process
    bool _start_time_set = false;
    //! A start time for the next stream command
    uhd::time_spec_t _start_time;
    //! Pointer to the IHD Software Radio Protocol devive
    ::ihd::ipsolon_isrp::sptr _isrp;
};

} // namespace ihd
} // namespace gr

#endif /* INCLUDED_IHD_IHD_SOURCE_IMPL_H */
