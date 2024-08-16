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

class ipsolon_stream : public uhd::rx_streamer {

public:
    ipsolon_stream() = default;

    static constexpr size_t UDP_PACKET_SIZE    =  16400;
    static constexpr size_t BYTES_PER_SAMPLE   =      2;
    static constexpr size_t BYTES_PER_IQ_PAIR  = (BYTES_PER_SAMPLE * 2);
    static constexpr size_t PACKET_HEADER_SIZE = (chdr_header::CHDR_W + sizeof(uint64_t)); /* CHDR + timestamp */
    static constexpr size_t SAMPLES_PER_PACKET = ((UDP_PACKET_SIZE - PACKET_HEADER_SIZE) / BYTES_PER_IQ_PAIR);

    typedef std::shared_ptr<ipsolon_stream> sptr;

    class stream_type {
    public:
        static inline const std::string STREAM_FORMAT_KEY = "stream_format";
        static inline const std::string IQ_STREAM         = "IQ";
        static inline const std::string FFT_STREAM        = "FFT";

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

        bool modeEquals(const std::string& mode) {
            return mode == _stream_mode_str;
        }

    private:
        std::set<std::string> _modes { IQ_STREAM, FFT_STREAM };
        std::string _stream_mode_str;
    };

    [[nodiscard]] size_t get_num_channels() const override;
    [[nodiscard]] size_t get_max_num_samps() const override;
    size_t recv(const buffs_type& buffs, size_t nsamps_per_buff, uhd::rx_metadata_t& metadata,
        double timeout, bool one_packet) override;
    void issue_stream_cmd(const uhd::stream_cmd_t& stream_cmd) override;

    static sptr make(const uhd::stream_args_t& stream_cmd, const uhd::device_addr_t& _device_addr);
};

} // ihd

#endif //IPSOLON_STREAM_HPP
