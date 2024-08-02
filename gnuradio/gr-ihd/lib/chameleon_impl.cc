/* -*- c++ -*- */
/*
 * Copyright 2024 Ipsolon.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "chameleon_impl.h"
#include <gnuradio/io_signature.h>


namespace gr::ihd {

constexpr size_t n_chan = 1;
constexpr int NUMBER_OF_STREAM_ITEMS = 8; /* Not at all correct, just getting GNU Radio to run */

chameleon::sptr chameleon::make(double center_freq, std::string ip_addr)
{
    return gnuradio::make_block_sptr<chameleon_impl>(center_freq, ip_addr);
}

bool chameleon_impl::start()
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

bool chameleon_impl::stop()
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
chameleon_impl::chameleon_impl(double center_freq, std::string ip_addr)
    : gr::sync_block("chameleon",
                     gr::io_signature::make(0,0, 0),
                     gr::io_signature::make(0,n_chan,NUMBER_OF_STREAM_ITEMS)),
      _issue_stream_cmd_on_start(true),
      _stream_args("sc16", "sc16")
{
    d_logger->debug("Start constructing IHD");
    _center_freq = center_freq;
    std::string args("addr=192.168.10.200");
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
}

/*
 * Our virtual destructor.
 */
chameleon_impl::~chameleon_impl() = default;

#define PACKET_SIZE 4088
int chameleon_impl::work(int noutput_items,
                         gr_vector_const_void_star& input_items,
                         gr_vector_void_star& output_items)
{
    uhd::rx_metadata_t md;
    auto ret = _streamer->recv(output_items, noutput_items, md, 5);

    static int work_count = 0;
    if((work_count++ % 1000) == 0 || noutput_items != (int)ret) {
        printf("Working.  number output_items:%d ret:%zu\n", noutput_items, ret);
    }
    // Tell runtime system how many output items we produced.
    return (int)ret;
}

} // namespace gr::ihd

