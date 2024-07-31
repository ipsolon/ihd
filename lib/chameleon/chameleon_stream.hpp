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
    // FIXME - too much, move this to its own hpp/cpp file
    class chameleon_packet {
    public:
        explicit chameleon_packet(size_t ps) :  packet_size(ps),
                                                data_size(ps - ipsolon_stream::PACKET_HEADER_SIZE),
                                                nIQ(data_size/2),
                                                pos(0),
                                                samples(nullptr)
        {
            packet_mem = static_cast<uint8_t *>(malloc(packet_size));
            if(packet_mem == nullptr) {
                THROW_MALLOC_ERROR();
            } else {
                samples = reinterpret_cast<uint16_t *>(packet_mem + ipsolon_stream::PACKET_HEADER_SIZE);
            }
        }

        ~chameleon_packet() {
            if (packet_mem != nullptr) {
                free(packet_mem);
            }
        }

        [[nodiscard]]
        chdr_header getChdr() const {
            if (packet_mem == nullptr) {
                return chdr_header(0);
            } else {
                return chdr_header(
                        (uint64_t) packet_mem[0] |
                        (uint64_t) packet_mem[1] << 8  |
                        (uint64_t) packet_mem[2] << 16 |
                        (uint64_t) packet_mem[3] << 24 |
                        (uint64_t) packet_mem[4] << 32 |
                        (uint64_t) packet_mem[5] << 40 |
                        (uint64_t) packet_mem[6] << 48 |
                        (uint64_t) packet_mem[7] << 56);
            }
        }

        [[nodiscard]]
        uint64_t getTimestamp() const {
            if(packet_mem == nullptr) {
                return 0;
            } else {
                return (uint64_t) packet_mem[0]       |
                       (uint64_t) packet_mem[1] << 8  |
                       (uint64_t) packet_mem[2] << 16 |
                       (uint64_t) packet_mem[3] << 24 |
                       (uint64_t) packet_mem[4] << 32 |
                       (uint64_t) packet_mem[5] << 40 |
                       (uint64_t) packet_mem[6] << 48 |
                       (uint64_t) packet_mem[7] << 56;;
            }
        }

        [[nodiscard]] size_t getPacketSize() const {
            return packet_size;
        }

        [[nodiscard]] size_t getDataSize() const {
            return data_size;
        }

        size_t getPos() const {
            return pos;

        }

        void setPos(size_t position) {
            pos = std::min(position, data_size - 1);
        }

        size_t getSamples(chameleon_stream::chameleon_data_type *buff, size_t n_samples) {
            size_t n = std::min(n_samples, nIQ - pos);

            for (size_t i = 0; i < n; i++) {
                size_t s = (i * 2);
                buff[i] = chameleon_stream::chameleon_data_type(samples[s], samples[s+1]);
            }
            return n;
        }

        [[nodiscard]]
        uint8_t *getPacketMem() const {
            return packet_mem;
        }

    private:
        size_t packet_size;
        size_t data_size;
        size_t nIQ;
        size_t pos;
        uint8_t *packet_mem;
        uint16_t *samples;

    };

    static constexpr int vita_port = 9090;
    static constexpr size_t bytes_per_sample = ipsolon_stream::BYTES_PER_SAMPLE;
    static constexpr size_t bytes_per_packet = ipsolon_stream::UDP_PACKET_SIZE;
    static constexpr size_t max_sample_per_packet = bytes_per_packet / bytes_per_sample;
    static constexpr size_t buffer_mem_size = (16 * 1024 * 1024); /* The memory allocated to store received UDP packets */
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
};

} // ihd

#endif //CHAMELEON_STREAM_HPP
