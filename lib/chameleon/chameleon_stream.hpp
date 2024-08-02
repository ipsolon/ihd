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

#include "ipsolon_stream.hpp"
#include "ipsolon_chdr_header.h"
#include "exception.hpp"

namespace ihd {

class chameleon_packet;

class chameleon_stream : public ipsolon_stream {
public:
    static const size_t default_timeout = 5;
    typedef std::complex<uint16_t> chameleon_data_type;

    explicit chameleon_stream(const uhd::stream_args_t& stream_cmd, const uhd::device_addr_t& device_addr);
    ~chameleon_stream() override;

    [[nodiscard]] size_t get_num_channels() const override;
    [[nodiscard]] size_t get_max_num_samps() const override;
    size_t recv(const buffs_type& buffs, const size_t nsamps_per_buff, uhd::rx_metadata_t& metadata,
        const double timeout, const bool one_packet) override;
    void issue_stream_cmd(const uhd::stream_cmd_t& stream_cmd) override;

private:
    static constexpr int vita_port = 9090;
    static constexpr size_t bytes_per_sample = ipsolon_stream::BYTES_PER_SAMPLE;
    static constexpr size_t bytes_per_packet = ipsolon_stream::UDP_PACKET_SIZE;
    static constexpr size_t max_sample_per_packet = bytes_per_packet / bytes_per_sample;
    static constexpr size_t buffer_mem_size = (4 * 1024 * 1024); /* The memory allocated to store received UDP packets */
    static constexpr size_t buffer_packet_cnt = buffer_mem_size / bytes_per_sample;

    timeval _vita_port_timeout = {default_timeout, 0};

    /* Free Queue and Sample Queue
     * Receiver: Take from free queue, receive message, place in sample queue.
     *           When free is empty, steal from samples queue.
     * Consumer: Take from sample queue, process samples, place in free queue when done
     */
    std::queue<chameleon_packet*> q_free_packets;
    std::mutex mtx_free_queue;
    std::condition_variable cv_free_queue;

    std::queue<chameleon_packet*> q_sample_packets;
    std::mutex mtx_sample_queue;
    std::condition_variable cv_sample_queue;

    size_t _nChans{};
    uint32_t _chanMask{};
    chameleon_fw_commander _commander;
    int _socket_fd{};

    chameleon_packet *_current_packet;

    typedef struct receive_thread_context {
        bool run;
        int socket_fd;

        std::queue<chameleon_packet*> *q_free;
        std::mutex *mtx_free;
        std::condition_variable *cv_free;

        std::queue<chameleon_packet*> *q_samples;
        std::mutex *mtx_samples;
        std::condition_variable *cv_samples;

    } receive_thread_context_t;

    receive_thread_context_t _receive_thread_context;
    std::thread _recv_thread;

    void start_stream();
    void stop_stream();

    void open_socket();

    static void receive_thread_func(const receive_thread_context_t *rtc);
    size_t get_packet_data(size_t n, chameleon_data_type *buff);
};

} // ihd

#endif //CHAMELEON_STREAM_HPP
