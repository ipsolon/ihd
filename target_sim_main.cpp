/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/
#include <iostream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "safe_main.hpp"
#include "chameleon_fw_common.h"

#define RECV_TIMEOUT_SECS 20

std::queue<chameleon_fw_comms_t> buffer;
std::mutex mtx;
std::condition_variable cv;

void process_thread_func() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !buffer.empty(); });
        chameleon_fw_comms_t request = buffer.front();
        buffer.pop();
        std::cout << "Process Thread"
           << " flags:0x" << std::hex << request.flags
           << " sequence:0x" << std::hex << request.sequence
           << " addr:0x" << std::hex << request.addr
           << " chan:"   << std::dec << request.tune.chan
           << " Freq:"   << std::dec << request.tune.freq
           << std::dec << std::endl;
        lock.unlock();
    }
}

void recv_thread_func(int sockfd)
{
    std::cout << "thread function ";
    std::cout << "the socket is = " << sockfd << std::endl;

    socklen_t len;
    auto request = chameleon_fw_comms_t();
    sockaddr_in servaddr{};
    servaddr.sin_port = CHAMELEON_FW_COMMS_UDP_PORT;
    std::cout << "Listning on port:" << servaddr.sin_port << std::endl;

    int n = 0;
    while (n >= 0) {
        n = recvfrom(sockfd, &request, sizeof(request),
                               0, (struct sockaddr*)&servaddr,
                               &len);
        if (n > 0) {
            std::cout << "Recieved"
                << " flags:0x" << std::hex << request.flags
                << " bytes:" << std::dec << n
                << " sequence:0x" << std::hex << request.sequence
                << " addr:0x" << std::hex << request.addr
                << " data:"   << std::dec << request.tune.chan
                << " data:"   << std::dec << request.tune.freq
                << std::dec << std::endl;
            std::unique_lock<std::mutex> lock(mtx);
            buffer.push(request);
            cv.notify_one();
            lock.unlock();
        } else if (n < 0 && errno == ETIMEDOUT ) {
            n = 0;
            std::cout << "Timeout waiting for command" << std::endl;
        }
    }
}

int open_socket()
{
    int err = 0;
    int sockfd = -1;

    // Creating socket file descriptor
    err = socket(AF_INET, SOCK_DGRAM, 0);
    if (err < 0) {
        perror("socket creation failed");
    }
    else {
        sockfd = err;

        sockaddr_in servaddr{};
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(CHAMELEON_FW_COMMS_UDP_PORT);
        servaddr.sin_addr.s_addr = INADDR_ANY;
        err = bind(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr));
        if (err < 0) {
            perror("bind failed");
        }
    }
    if (!err) {
        timeval tv;
        tv.tv_sec = RECV_TIMEOUT_SECS;
        tv.tv_usec = 0;
        err = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        if (err < 0) {
            perror("Socket timeout set error");
        }
    }
    if (err && sockfd > -1) {
        close(sockfd);
    }
    return err ? err : sockfd;
}

int IHD_SAFE_MAIN(int argc, char *argv[])
{
    int err = 0;
    int sockfd = -1;

    sockfd = open_socket();
    if (sockfd < 0) {
        err = -1;
    }

    std::thread recv_thread(&recv_thread_func, sockfd);
    std::thread process_thread(&process_thread_func);

    recv_thread.join();
    process_thread.join();
    if (sockfd > -1) {
        close(sockfd);
    }
    return err;
}
