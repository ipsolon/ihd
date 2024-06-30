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
constexpr size_t samples = 4096;
constexpr size_t nchan = 1;

ihd_source::sptr ihd_source::make(double center_freq)
{
    return gnuradio::make_block_sptr<ihd_source_impl>(center_freq);
}

bool ihd_source_impl::start()
{
    if (_issue_stream_cmd_on_start) {
        // Start the streamers
        ::uhd::stream_cmd_t stream_cmd(::uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
        if (_start_time_set) {
            stream_cmd.stream_now = false;
            stream_cmd.time_spec = _start_time;
            _start_time_set = false;
        } else {
            stream_cmd.stream_now = true;
        }

        d_logger->debug("Sending start stream command...");
        _streamer->issue_stream_cmd(stream_cmd);
    } else {
        d_logger->debug("Starting RX streamer without stream command...");
    }
    return true;
}

bool ihd_source_impl::stop()
{
    // If we issue a stream command on start, we also issue it on stop
    if (_issue_stream_cmd_on_start) {
        ::uhd::stream_cmd_t stream_cmd(::uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
        _streamer->issue_stream_cmd(stream_cmd);
    }
    // @TODO implement: flush();
    return true;
}

/*
 * The private constructor
 */
ihd_source_impl::ihd_source_impl(double center_freq)
    : gr::sync_block("ihd_source",
                     gr::io_signature::make(0, 0, 0),
                     gr::io_signature::make(
                     1 /* min outputs */, 1 /*max outputs */, nchan)),
    _issue_stream_cmd_on_start(true),
    _stream_args("sc16", "sc16")
{
    _center_freq = center_freq;
    std::string args("addr=10.75.42.15");
    size_t channel;

    _isrp = ::ihd::ipsolon_isrp::make(args);
    uhd::tune_request_t tune_request{};
    tune_request.rf_freq = _center_freq;
    constexpr size_t chan = 0;
    _isrp->set_rx_freq(tune_request, chan);

    std::vector<size_t> channel_nums;
    channel_nums.push_back(channel);
    _stream_args.channels = channel_nums;
    _streamer = _isrp->get_rx_stream(_stream_args);


#if 0
    sleep(5);
    stream_cmd.stream_mode = uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS;
    rx_stream->issue_stream_cmd(stream_cmd);
#endif
}

/*
 * Our virtual destructor.
 */
ihd_source_impl::~ihd_source_impl() {}

int ihd_source_impl::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
     std::vector<uint8_t> buffs = std::vector<uint8_t>(samples);
    output_items.push_back(&buffs);

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace ihd */
} /* namespace gr */
