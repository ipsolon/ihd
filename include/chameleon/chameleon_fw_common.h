//
// Created by jmeyers on 6/26/24.
//

#ifndef CHAMELEON_FW_COMMON_H
#define CHAMELEON_FW_COMMON_H


typedef struct {
    size_t chan;
    double freq;
} chameleon_fw_cmd_tune_t;

typedef struct {
    uint32_t flags;
    uint32_t sequence;
    uint32_t addr;
    std::variant<uint64_t, double, chameleon_fw_cmd_tune_t> data;
} chameleon_fw_comms_t;

typedef enum {
    CHAMELEON_FW_COMMS_FLAGS_ACK        = (1 << 0),
    CHAMELEON_FW_COMMS_FLAGS_ERROR      = (1 << 1),
    CHAMELEON_FW_COMMS_FLAGS_READ       = (1 << 2),
    CHAMELEON_FW_COMMS_FLAGS_WRITE      = (1 << 3),
} chameleon_flags_t;

typedef enum {
    CHAMELEON_FW_COMMS_CMD_NOOP,
    CHAMELEON_FW_COMMS_CMD_TUNE_FREQ,
    CHAMELEON_FW_COMMS_CMD_STREAM_START,
    CHAMELEON_FW_COMMS_CMD_STREAM_STOP,
} chameleon_command_t;

#define CHAMELEON_FW_COMMS_UDP_PORT  49152
#define CHAMELEON_VITA_UDP_PORT      49153
#define CHAMELEON_FPGA_PROG_UDP_PORT 49157
#define CHAMELEON_FPGA_READ_UDP_PORT 49159

#define CHAMELEON_FW_COMMS_IPADDR "10.75.42.15"

#endif //CHAMELEON_FW_COMMON_H
