/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/


#ifndef CHAMELEON_STREAM_HPP
#define CHAMELEON_STREAM_HPP
#include <thread>
#include <chameleon_fw_commander.hpp>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <netinet/in.h>

#include "ipsolon_rx_stream.hpp"
#include "ipsolon_chdr_header.h"

// FIXME
#define DEFAULT_BUFFER_SIZE (4 * 1024 * 1024)

namespace ihd {
    class chameleon_packet;

    class chameleon_rx_stream : public ipsolon_rx_stream {
    public:
        typedef std::complex<int16_t> chameleon_data_type;

        explicit chameleon_rx_stream(const uhd::stream_args_t &stream_cmd, const uhd::device_addr_t &device_addr);

        ~chameleon_rx_stream() override;

        [[nodiscard]] size_t get_num_channels() const override;

        [[nodiscard]] size_t get_max_num_samps() const override;

        size_t recv(const buffs_type &buffs, size_t nsamps_per_buff, uhd::rx_metadata_t &metadata,
                    double timeout, bool one_packet) override;

        void issue_stream_cmd(const uhd::stream_cmd_t &stream_cmd) override;

    protected:
        virtual void send_rx_cfg_set_cmd(uint32_t chanMask) = 0;
        virtual size_t get_max_num_samps() = 0;

        stream_type _stream_type;
        size_t _max_samples_per_packet;
        size_t _buffer_packet_cnt;
        chameleon_fw_commander _commander;

    private:
        static const std::string DEFAULT_VITA_IP_STR;
        static constexpr uint32_t DEFAULT_VITA_IP = INADDR_ANY;
        static constexpr uint32_t DEFAULT_VITA_PORT = 9090;
        static constexpr size_t DEFAULT_TIMEOUT = 30;

        std::string _vita_ip_str;
        in_addr_t _vita_ip;
        uint16_t _vita_port;
        uint32_t _stream_id{};
        static constexpr uint32_t DEFAULT_PACKET_SIZE = 8192;
        static constexpr uint32_t DEFAULT_IQ_BUFFER_MEM_SIZE = 0x28C58000;

        size_t _bytes_per_packet;

        size_t _buffer_mem_size = (DEFAULT_BUFFER_SIZE); /* The memory allocated to store received UDP packets */

        timeval _vita_port_timeout = {DEFAULT_TIMEOUT, 0};

        /* Free Queue and Sample Queue
         * Receiver: Take from free queue, receive message, place in sample queue.
         *           When free is empty, steal from samples queue.
         * Consumer: Take from sample queue, process samples, place in free queue when done
         */
    protected:
        std::queue<chameleon_packet *> q_free_packets;
        std::mutex mtx_free_queue;
    private:
        std::condition_variable cv_free_queue;

        std::queue<chameleon_packet *> q_sample_packets;
        std::mutex mtx_sample_queue;
        std::condition_variable cv_sample_queue;

        std::mutex mtx_stream;

        size_t _nChans{};
        uint32_t _chanMask{};
        int _socket_fd{};

        chameleon_packet *_current_packet;
        bool _first_packet{};
        /** Last sequence number received - compare to current to detect missing packets */
        uint16_t _previous_seq{};

        typedef struct receive_thread_context {
            bool run;
            int socket_fd;

            std::queue<chameleon_packet *> *q_free;
            std::mutex *mtx_free;
            std::condition_variable *cv_free;

            std::queue<chameleon_packet *> *q_samples;
            std::mutex *mtx_samples;
            std::condition_variable *cv_samples;
        } receive_thread_context_t;

        receive_thread_context_t _receive_thread_context;
        std::thread _recv_thread;

        void start_stream();

        void stop_stream();

        void open_socket();

        static void receive_thread_func(receive_thread_context_t *rtc);

        size_t get_packet_data(size_t n, chameleon_data_type *buff, uhd::rx_metadata_t &metadata, uint64_t timeout_ms);
    };
} // ihd

#endif //CHAMELEON_STREAM_HPP
