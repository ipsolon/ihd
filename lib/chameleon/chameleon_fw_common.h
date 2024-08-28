/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CHAMELEON_FW_COMMON_H
#define CHAMELEON_FW_COMMON_H

#include <sstream>

#define CHAMELEON_FW_COMMS_UDP_PORT  64000
#define CHAMELEON_FW_CMD_MAX_SIZE     9000

namespace ihd {
    class chameleon_fw_cmd {
    public:
        explicit chameleon_fw_cmd(const char *cmd) : _cmd(cmd) {}
        virtual const char *to_command_string() = 0;

        virtual ~chameleon_fw_cmd() = default;

    protected:
        const char *_cmd{};
        std::string _command_string;
        std::string _reply_string;
    };

    class chameleon_fw_cmd_tune : public chameleon_fw_cmd {
    public:
        chameleon_fw_cmd_tune(size_t c, uint64_t f) :
            chameleon_fw_cmd("set_freq"),
            chan(c), freq(f) {}

        const char *to_command_string() override {
            std::stringstream ss;
            ss << _cmd << " " << freq;
            _command_string = ss.str();
            return _command_string.c_str();
        }

    private:
        size_t chan{};
        uint64_t freq{};
    };

    class chameleon_fw_cmd_stream : public chameleon_fw_cmd {
    public:
        chameleon_fw_cmd_stream(uint32_t mask, bool enable) :
            chameleon_fw_cmd("stream"),
            _chan_mask(mask), _enable(enable) {}

        const char *to_command_string() override { return nullptr; /* TODO - implement stream command */ }
    private:
        uint32_t _chan_mask{};
        bool _enable{};
    };

    class chameleon_fw_comms {
    public:
        chameleon_fw_comms(uint32_t flags, uint32_t sequence, uint32_t cmd, std::unique_ptr<chameleon_fw_cmd> command) :
            _flags(flags), _sequence(sequence), _cmd(cmd), _command(std::move(command)) {}

        chameleon_fw_comms(uint32_t flags, uint32_t cmd, std::unique_ptr<chameleon_fw_cmd> command) :
                _flags(flags), _sequence(0), _cmd(cmd), _command(std::move(command)) {}

        [[nodiscard]]
        uint32_t getFlags() const {
            return _flags;
        }

        void setFlags(uint32_t flags) {
            _flags = flags;
        }

        [[nodiscard]]
        uint32_t getSequence() const {
            return _sequence;
        }

        void setSequence(uint32_t sequence) {
            _sequence = sequence;
        }

        [[nodiscard]]
        uint32_t getCmd() const {
            return _cmd;
        }

        void setCmd(uint32_t cmd) {
            _cmd = cmd;
        }

        const char *getCommandString() {
            return _command->to_command_string();
        }

        virtual ~chameleon_fw_comms() = default;

    private:
        uint32_t _flags{};
        uint32_t _sequence{};
        uint32_t _cmd{};
        std::unique_ptr<chameleon_fw_cmd> _command{};
    };

    typedef enum {
        CHAMELEON_FW_COMMS_FLAGS_ACK   = 1 << 0,
        CHAMELEON_FW_COMMS_FLAGS_NCK   = 1 << 1,
        CHAMELEON_FW_COMMS_FLAGS_ERROR = 1 << 2,
        CHAMELEON_FW_COMMS_FLAGS_READ  = 1 << 3,
        CHAMELEON_FW_COMMS_FLAGS_WRITE = 1 << 4,
    } chameleon_flags_t;

    typedef enum {
        CHAMELEON_FW_COMMS_CMD_TUNE_FREQ,
        CHAMELEON_FW_COMMS_CMD_STREAM_CMD,
    } chameleon_command_t;

}

#endif //CHAMELEON_FW_COMMON_H
