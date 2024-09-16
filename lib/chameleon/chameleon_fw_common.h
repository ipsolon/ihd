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
            ss << _cmd << " chan=" << chan << "," " freq=" << freq;
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
        chameleon_fw_comms(uint32_t sequence, std::unique_ptr<chameleon_fw_cmd> command) :
                _sequence(sequence), _command(std::move(command)) {}

        chameleon_fw_comms(std::unique_ptr<chameleon_fw_cmd> command) :
                _sequence(0), _command(std::move(command)) {}

        [[nodiscard]]
        uint32_t getSequence() const {
            return _sequence;
        }

        void setSequence(uint32_t sequence) {
            _sequence = sequence;
        }

        std::string getCommandString() {
            std::stringstream ss;
            printf("The sequence number:%d\n", _sequence);
            if (_sequence > 0) {
                ss << _sequence << " ";
            }
            printf("The sequence string:%s\n", ss.str().c_str());
            ss << _command->to_command_string();
            printf("The string:%s\n", ss.str().c_str());
            return ss.str();
        }

        virtual ~chameleon_fw_comms() = default;

        void setResponse(const char *response) {
            printf("TODO: parse response:%s\n", response);
        }

        void setResponseTimedout() {
            printf("Timeout waiting for response\n");
        }

    private:
        uint32_t _sequence{};
        std::unique_ptr<chameleon_fw_cmd> _command{};
    };
}

#endif //CHAMELEON_FW_COMMON_H
