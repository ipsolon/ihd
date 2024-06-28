//
// Created by jmeyers on 6/28/24.
//

#ifndef CHAMELEON_FW_CMD_HPP
#define CHAMELEON_FW_CMD_HPP
#include <atomic>
#include <uhd/device.hpp>
#include <uhd/transport/udp_simple.hpp>
#include "chameleon_fw_common.h"

namespace ihd {

class chameleon_fw_commander {
public:
    explicit chameleon_fw_commander(const uhd::device_addr_t& dev_addr);
    size_t send_request(chameleon_fw_comms_t &request) const;
private:
    uhd::transport::udp_simple::sptr _udp_cmd_port{};
    static std::atomic<size_t> _seq;
};

} // ihd

#endif //CHAMELEON_FW_CMD_HPP
