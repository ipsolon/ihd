/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef IPSOLON_STREAM_HPP
#define IPSOLON_STREAM_HPP

#include <uhd/stream.hpp>
#include <set>
#include "ipsolon_chdr_header.h"

namespace ihd {

class ipsolon_rx_stream : public uhd::rx_streamer {

public:
    ipsolon_rx_stream() = default;

    static constexpr size_t BYTES_PER_SAMPLE = 2;
    static constexpr size_t PACKET_HEADER_SIZE = (chdr_header::CHDR_W + sizeof(uint64_t)); /* CHDR + timestamp */
    static constexpr size_t BYTES_PER_IQ_PAIR  = (BYTES_PER_SAMPLE * 2);

    typedef std::shared_ptr<ipsolon_rx_stream> sptr;

    class stream_type {
    public:
        static const std::string STREAM_FORMAT_KEY;
        static const std::string IQ_STREAM;
        static const std::string PSD_STREAM;
        static const std::string FFT_STREAM;

        static const std::string STREAM_DEST_IP_KEY;
        static const std::string STREAM_DEST_PORT_KEY;

        static const std::string FFT_AVG_COUNT_KEY;
        static const std::string FFT_SIZE_KEY;

        explicit stream_type(const std::string& st)
        {
            if (_modes.find(st) == _modes.end()) {
                throw uhd::key_error("Invalid stream mode:" + st);
            }
            _stream_mode_str = st;
        };
        [[nodiscard]]
        const std::string &getStreamMode() const {
            return _stream_mode_str;
        }

        bool modeEquals(const std::string& mode) const {
            return mode == _stream_mode_str;
        }

    private:
        std::set<std::string> _modes { IQ_STREAM, PSD_STREAM, FFT_STREAM};
        std::string _stream_mode_str;
    };

    static sptr make(const uhd::stream_args_t& stream_cmd, const uhd::device_addr_t& _device_addr);
};

} // ihd

#endif //IPSOLON_STREAM_HPP
