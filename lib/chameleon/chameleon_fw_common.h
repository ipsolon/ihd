/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CHAMELEON_FW_COMMON_H
#define CHAMELEON_FW_COMMON_H

#define CHAMELEON_FW_CMD_MAX_DATA_SIZE 16
#define CHAMELEON_FW_CMD_PACKET_SIZE ((sizeof(uint32_t) * 3) + CHAMELEON_FW_CMD_MAX_DATA_SIZE)

#pragma pack(push, 1)
typedef struct { // Controls the maximum union size -- see static assert below
    uint8_t v[CHAMELEON_FW_CMD_MAX_DATA_SIZE];
} chameleon_fw_cmd_data_t;

typedef struct {
    size_t chan;
    double freq;
} chameleon_fw_cmd_tune_t;

typedef struct {
    size_t chans;
    bool enable;
} chameleon_fw_cmd_stream_t;

typedef struct {
    uint32_t flags;
    uint32_t sequence;
    uint32_t addr;
    union {
        chameleon_fw_cmd_data_t data;
        chameleon_fw_cmd_tune_t tune;
        chameleon_fw_cmd_stream_t stream;
    };
} chameleon_fw_comms_t;
#pragma pack(pop)

static_assert (sizeof(chameleon_fw_comms_t) == CHAMELEON_FW_CMD_PACKET_SIZE, "Comms structure size is not correct");


typedef enum {
    CHAMELEON_FW_COMMS_FLAGS_ACK   = 1 << 0,
    CHAMELEON_FW_COMMS_FLAGS_ERROR = 1 << 1,
    CHAMELEON_FW_COMMS_FLAGS_READ  = 1 << 2,
    CHAMELEON_FW_COMMS_FLAGS_WRITE = 1 << 3,
} chameleon_flags_t;

typedef enum {
    CHAMELEON_FW_COMMS_CMD_NOOP,
    CHAMELEON_FW_COMMS_CMD_TUNE_FREQ,
    CHAMELEON_FW_COMMS_CMD_STREAM_CMD,
} chameleon_command_t;

#define CHAMELEON_FW_COMMS_UDP_PORT  49152
#define CHAMELEON_VITA_UDP_PORT      49153
#define CHAMELEON_FPGA_PROG_UDP_PORT 49157
#define CHAMELEON_FPGA_READ_UDP_PORT 49159

#endif //CHAMELEON_FW_COMMON_H
