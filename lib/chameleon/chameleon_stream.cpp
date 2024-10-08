/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>

#include <uhd/transport/udp_simple.hpp>

#include "chameleon_fw_common.hpp"
#include "chameleon_stream.hpp"
#include "chameleon_packet.hpp"

#define IMPLEMENTED_CMD_PORT 0 /* Command server not yet implemented */

using namespace ihd;

chameleon_stream::chameleon_stream(const uhd::stream_args_t& stream_cmd, const uhd::device_addr_t& device_addr) :
    _nChans(stream_cmd.channels.size()), _commander(device_addr), _chanMask(0), _receive_thread_context{},
    _current_packet(nullptr)
{
    if(stream_cmd.cpu_format != "sc16") {
        THROW_VALUE_NOT_SUPPORTED_ERROR(stream_cmd.args.to_string());
    }
    if(stream_cmd.otw_format != "sc16") {
        THROW_VALUE_NOT_SUPPORTED_ERROR(stream_cmd.args.to_string());
    }
    for(const size_t& chan : stream_cmd.channels) {
        _chanMask |= 1 << chan; /* Channels indexed at zero */
    }
    try {
        std::string str = stream_cmd.args[ipsolon_stream::stream_type::STREAM_FORMAT_KEY];
        stream_type st(str);
        if (st.modeEquals(stream_type::FFT_STREAM)) {
            // FIXME - implement abstract recv class to handle IQ vs FFT - instantiate it here
            printf("Create FFT stream\n");
        } else {
            printf("Create IQ stream\n");
        }

    } catch (const uhd::key_error&) {
        printf("No stream format specified\n");
    }
    open_socket();

    _receive_thread_context.run = false;
    _receive_thread_context.q_free = &q_free_packets;
    _receive_thread_context.q_samples = &q_sample_packets;
    _receive_thread_context.mtx_free = &mtx_free_queue;
    _receive_thread_context.mtx_samples = &mtx_sample_queue;
    _receive_thread_context.cv_free = &cv_free_queue;
    _receive_thread_context.cv_samples = &cv_sample_queue;
    _receive_thread_context.socket_fd = _socket_fd;

    /* Fill the free queue */
    std::lock_guard<std::mutex> lock(mtx_free_queue);
    for (int i = 0; i < buffer_packet_cnt; ++i) {
        auto cp = new chameleon_packet(bytes_per_packet);
        q_free_packets.push(cp);
    }
}

chameleon_stream::~chameleon_stream()
{
    while (!q_free_packets.empty()) {
        chameleon_packet *pk = q_free_packets.front();
        q_free_packets.pop();
        free(pk);
    }
    while (!q_sample_packets.empty()) {
        chameleon_packet *pk = q_sample_packets.front();
        q_sample_packets.pop();
        free(pk);
    }
}

size_t chameleon_stream::get_num_channels() const
{
    return _nChans;
}

size_t chameleon_stream::get_max_num_samps() const
{
    return max_sample_per_packet;
}

size_t chameleon_stream::get_packet_data(size_t n_samples, chameleon_data_type *buff, uhd::rx_metadata_t& metadata)
{
    std::lock_guard<std::mutex> stream_lock(mtx_stream);
    static int count = 0;
    count++;

    if (_current_packet == nullptr) {
        std::unique_lock<std::mutex> lock(mtx_sample_queue);
        cv_sample_queue.wait(lock, [this] { return !q_sample_packets.empty(); });
        _current_packet = q_sample_packets.front();
        q_sample_packets.pop();

        metadata.reset();
        metadata.has_time_spec = true;
        metadata.time_spec = uhd::time_spec_t(
                static_cast<double>(_current_packet->getTimestamp())/1000000000);

        uint16_t seq = _current_packet->getCHDR().get_seq_num();
        uint16_t expected = _previous_seq + 1;
        metadata.out_of_sequence = (!_first_packet) && expected != seq;
        if (metadata.out_of_sequence) {
            metadata.error_code = uhd::rx_metadata_t::ERROR_CODE_OVERFLOW;
            printf("Previous seq:%x Current:%x missing:%d count:%d\n",
                   _previous_seq, seq, seq - _previous_seq, count);
        }
        _first_packet = false;
        _previous_seq = seq;
        lock.unlock();
    } else {
        metadata.fragment_offset = _current_packet->getPos();
    }

    n_samples = _current_packet->getSamples(buff, n_samples);

    if (_current_packet->endOfPacket()) {
        std::lock_guard<std::mutex> free_lock(mtx_free_queue);
        q_free_packets.push(_current_packet);
        _current_packet = nullptr;
        cv_free_queue.notify_one();
    } else {
        metadata.more_fragments = true;
    }
    return n_samples;
}

size_t chameleon_stream::recv(const buffs_type& buffs, const size_t nsamps_per_buff, uhd::rx_metadata_t& metadata,
                              const double timeout, const bool one_packet)
{
    int err = 0;
    size_t n_samples = 0;

    /* FIXME - do a proper C++ cast here */
    auto *output_array = (chameleon_data_type *)(buffs[0]);
    if(output_array == nullptr) {
        THROW_TYPE_ERROR();
    }

    while (n_samples < nsamps_per_buff && !err) {
        size_t n = get_packet_data(nsamps_per_buff - n_samples, output_array + n_samples, metadata);
        if (n > 0) {
            n_samples += n;
        } else {
            fprintf(stderr, "Error getting samples\n");
            err = -1;
        }
    }
    return n_samples;
}

void chameleon_stream::receive_thread_func(const receive_thread_context *rtc)
{
    sockaddr_in server_addr{};
    socklen_t len;

    while(rtc->run) {
        std::unique_lock<std::mutex> lock_free(*rtc->mtx_free);
        (*rtc->cv_free).wait(lock_free, [&rtc] { return !rtc->q_free->empty(); });
        auto cp = rtc->q_free->front();
        lock_free.unlock();

        ssize_t n = 0;
        while (n == 0 && rtc->run) {
            n = recvfrom(rtc->socket_fd, cp->getPacketMem(), cp->getPacketSize(), 0,
                         (struct sockaddr *) &server_addr,&len);
            if (n > 0) {
                lock_free.lock();
                rtc->q_free->pop();
                lock_free.unlock();

                std::lock_guard<std::mutex> lock_samples(*rtc->mtx_free);
                cp->setPacketSize(n);
                rtc->q_samples->push(cp);
                rtc->cv_samples->notify_one();

            } else if (n < 0) {
                std::cout << "Receive error." << " errno:" << errno << ":" << strerror(errno) << std::endl;
            } else { // is zero
                std::cout << "Receive timeout" << std::endl;
            }
        }
    }
}

void chameleon_stream::issue_stream_cmd(const uhd::stream_cmd_t& stream_cmd)
{
    switch (stream_cmd.stream_mode) {
        case uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS : start_stream(); break;
        case uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS :  stop_stream(); break;
        case uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE :
        case uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_MORE :
        default:
            THROW_NOT_IMPLEMENTED_ERROR();
    }
}

void chameleon_stream::start_stream()
{
    _first_packet = true;
    _receive_thread_context.run = true;
    _recv_thread = std::thread([=] { receive_thread_func(&_receive_thread_context); });

#if IMPLEMENTED_CMD_PORT
    chameleon_fw_cmd_stream stream_cmd(_chanMask, true);
    auto request = chameleon_fw_comms(CHAMELEON_FW_COMMS_FLAGS_WRITE, CHAMELEON_FW_COMMS_CMD_STREAM_CMD, stream_cmd);
    _commander.send_request(request);
#else
    /* For now, you just send the radio 'anything' and it goes */
    std::string ipAddr = _commander.getIP();
    sockaddr_in radio_addr{};
    radio_addr.sin_addr.s_addr = inet_addr(ipAddr.c_str());
    radio_addr.sin_port = htons(vita_port);

    uint8_t go[] = {0x67,0x6F};
    int n = sendto(_socket_fd, go, sizeof(go), MSG_CONFIRM,
                   (const struct sockaddr *) &radio_addr, sizeof(radio_addr));
    if (n != sizeof(go)) {
        THROW_SOCKET_ERROR();
    }
#endif
}

void chameleon_stream::stop_stream()
{
#if IMPLEMENTED_CMD_PORT
    chameleon_fw_cmd_stream stream_cmd(_chanMask, false);
    auto request = chameleon_fw_comms(CHAMELEON_FW_COMMS_FLAGS_WRITE, CHAMELEON_FW_COMMS_CMD_STREAM_CMD, stream_cmd);
    _commander.send_request(request);
#endif
    _receive_thread_context.run = false;
    _recv_thread.join();
    std::lock_guard<std::mutex> free_lock(mtx_free_queue);
    std::lock_guard<std::mutex> sample_lock(mtx_sample_queue);
    while(!q_sample_packets.empty()) {
        q_free_packets.push(q_sample_packets.front());
        q_sample_packets.pop();
    }
}

void chameleon_stream::open_socket() {
    int err = 0;
    int sock_fd = -1;

    // Creating socket file descriptor
    err = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (err < 0) {
        perror("socket creation failed");
    } else {
        int broadcast = 1;
        sock_fd = err;
        err = setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast);
        if (err) {
            perror("setsockopt failed");
        }
    }
    if (!err) {
        sockaddr_in local_addr{};
        local_addr.sin_family = AF_INET;
        local_addr.sin_port = htons(vita_port);
        local_addr.sin_addr.s_addr = INADDR_ANY;
        err = bind(sock_fd, (const struct sockaddr *) &local_addr, sizeof(local_addr));
        if (err < 0) {
            perror("bind failed");
        }
    }
    if (!err) {
        err = setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &_vita_port_timeout, sizeof(_vita_port_timeout));
        if (err < 0) {
            perror("Socket timeout set error");
        }
    }
    if (err) {
        if (sock_fd > -1) {
            close(sock_fd);
        }
        throw std::runtime_error("Error opening socket");
    } else {
        _socket_fd = sock_fd;
    }
}
