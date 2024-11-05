/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "chameleon_fw_commander.hpp"

#include <utility>
#include "chameleon_fw_common.hpp"

namespace ihd {
    static std::atomic<size_t> _seq(1);

    chameleon_fw_commander::chameleon_fw_commander(uhd::device_addr_t  da) : _dev_addr(std::move(da))
    {
        _udp_cmd_port = uhd::transport::udp_simple::make_connected(_dev_addr["addr"],
                                                                   std::to_string(CHAMELEON_FW_COMMS_UDP_PORT));
    }
    int chameleon_fw_commander::send_request(chameleon_fw_comms &request, int timeout_ms) const
    {
        int err = 0;
        size_t ret = 0;

        request.setSequence(_seq++);
        std::string str = request.getCommandString();
        ret = _udp_cmd_port->send(boost::asio::buffer(str.c_str(), str.length()));
        if (ret != str.length()) {
            err = -1;
        } else if (timeout_ms > 0) {
            // Send passed and the caller wants to wait for a response
            char response[CHAMELEON_FW_CMD_MAX_SIZE] = {0};
            // TODO - check in a loop to make sure it the response this is looking for (somehow)
            try {
                ret = _udp_cmd_port->recv(boost::asio::buffer(response), ((double) timeout_ms / 1000.0));
                if (!ret) { // Timeout
                    request.setResponseTimedout();
                    err = -1;
                } else {
                    request.setResponse(response);
                }
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "Error: unknown exception" << std::endl;
            }                                                         \
        }
        return err;
    }

    const char *chameleon_fw_commander::getIP() {
        return _dev_addr["addr"].c_str();
    }
} // ihd