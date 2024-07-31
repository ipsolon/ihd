/* -*- c++ -*- */
/*
 * Copyright 2024 Ipsolon.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ihd_source_impl.h"
#include <gnuradio/io_signature.h>

namespace gr::ihd {

constexpr size_t samples = 4096;
constexpr size_t n_chan = 1;
constexpr size_t n_inputs = 1;
constexpr int NUMBER_OF_STREAM_ITEMS = 8; /* Not at all correct, just getting GNU Radio to run */

ihd_source::sptr ihd_source::make(double center_freq)
{
    return gnuradio::make_block_sptr<ihd_source_impl>(center_freq);
}

bool ihd_source_impl::start()
{
    d_logger->debug("IHD starting");
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
    d_logger->debug("IHD done starting");
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
ihd_source_impl::ihd_source_impl(double center_freq) :
                   gr::sync_block("ihd_source",
      gr::io_signature::make(0,0, 0),
     gr::io_signature::make(0,n_chan,NUMBER_OF_STREAM_ITEMS)),
      _issue_stream_cmd_on_start(true),
      _stream_args("sc16", "sc16")
{
    d_logger->debug("Start constructing IHD");
    _center_freq = center_freq;
    std::string args("addr=127.0.0.1");
    size_t channel = 0;

    _isrp = ::ihd::ipsolon_isrp::make(args);
    uhd::tune_request_t tune_request{};
    tune_request.rf_freq = _center_freq;
    constexpr size_t chan = 0;
    _isrp->set_rx_freq(tune_request, chan);

    std::vector<size_t> channel_nums;
    channel_nums.push_back(channel);
    _stream_args.channels = channel_nums;
    _streamer = _isrp->get_rx_stream(_stream_args);

    d_logger->debug("Done constructing IHD");

#if 0
    sleep(5);
    stream_cmd.stream_mode = uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS;
    rx_stream->issue_stream_cmd(stream_cmd);
#endif
}

/*
 * Our virtual destructor.
 */
ihd_source_impl::~ihd_source_impl() = default;

/* FIXME - Shared header? --  Adjust this when we settle on a packet size for sure */
#define NUMBER_OF_CHANNELS                1 /* We are limited to a single channel right now */
#define DEFAULT_UDP_PACKET_SIZE        8960 /* Fixed size by radio */
#define DEFAULT_BYTES_PER_SAMPLE          2 /* 16 Bits */
#define DEFAULT_BYTES_PER_IQ_PAIR      (DEFAULT_BYTES_PER_SAMPLE * 2)   /* 16 Bit I/Q = 4 Bytes */
#define DEFAULT_IQ_SAMPLES_PER_BUFFER ((DEFAULT_UDP_PACKET_SIZE - 16) / \
                                        DEFAULT_BYTES_PER_IQ_PAIR)      /* i.e. the number of IQ pairs, minus CHDR & timestamp */

int ihd_source_impl::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    int ret = std::min(noutput_items,DEFAULT_IQ_SAMPLES_PER_BUFFER);
    uhd::rx_metadata_t md;
    _streamer->recv(output_items, DEFAULT_IQ_SAMPLES_PER_BUFFER, md, 5);

    static int work_count = 0;
    if((work_count++ % 10000) == 0) printf("Working.  number output_items:%d ret:%d\n", noutput_items, ret);
    // Tell runtime system how many output items we produced.
    return ret;
}

} // namespace gr::ihd

