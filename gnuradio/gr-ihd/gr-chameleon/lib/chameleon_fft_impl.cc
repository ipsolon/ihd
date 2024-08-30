/* -*- c++ -*- */
/*
 * Copyright 2024 Ipsolon.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "chameleon_fft_impl.h"
#include <gnuradio/io_signature.h>

#include <utility>


const unsigned char wireshark_packet[4096 * 2 * 2] = {0};

namespace gr::chameleon {

using output_type = gr_complex;
chameleon_fft::sptr chameleon_fft::make(double center_freq, std::string ip_addr)
{
    return gnuradio::make_block_sptr<chameleon_fft_impl>(center_freq, ip_addr);
}

bool chameleon_fft_impl::start()
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

bool chameleon_fft_impl::stop()
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
chameleon_fft_impl::chameleon_fft_impl(double center_freq, std::string ip_addr)
    : gr::sync_block("chameleon_fft",
                     gr::io_signature::make(0, 0, 0),
                     gr::io_signature::make(1 /* min outputs */, 1 /*max outputs */, sizeof(output_type))),
      _center_freq(center_freq),
      _ipaddr(std::move(ip_addr)),
      _issue_stream_cmd_on_start(true),
      _stream_args("sc16", "sc16")
{
    std::stringstream ss;
    ss << "addr=" << _ipaddr;
    _isrp = ihd::ipsolon_isrp::make(ss.str());

    ::uhd::tune_request_t tune_request{};
    tune_request.rf_freq = _center_freq;
    size_t chan = 0;
    _isrp->set_rx_freq(tune_request, chan);

    std::vector<size_t> channel_nums;
    size_t channel = 0; // TODO implement channels
    channel_nums.push_back(channel);
    _stream_args.channels = channel_nums;
    _streamer = _isrp->get_rx_stream(_stream_args);
}

static void write_samples(output_type* output, size_t sample_per_buffer, std::complex<int16_t> *samples)
{
    for (size_t j = 0; j < sample_per_buffer; j++) {
        output[j] = samples[j];
    }
}

/*
 * Our virtual destructor.
 */
chameleon_fft_impl::~chameleon_fft_impl() = default;

int chameleon_fft_impl::work(int noutput_items,
                             gr_vector_const_void_star& input_items,
                             gr_vector_void_star& output_items)
{
    const int force_n_items = 4096;
    int ret = force_n_items;
    if (noutput_items < force_n_items) {
        ret = 0;
    } else {
        /************************************************************************
         * Allocate buffers
         ***********************************************************************/
        std::vector<std::complex<int16_t>*> buffs(1);
        // FFT size * 2 uint16's (Real and Imaginary components)
        std::complex<int16_t> p[force_n_items * sizeof(uint16_t) * 2];
        buffs[0] = p;
        uhd::rx_metadata_t md;
        _streamer->recv(buffs, force_n_items, md, 5);

        auto out = static_cast<output_type*>(output_items[0]);
        write_samples(out, force_n_items, p);
    }
    // Tell runtime system how many output items we produced.
    return ret;
}

} // namespace gr::chameleon

