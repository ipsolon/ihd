/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CHAMELEON_FW_COMMON_H
#define CHAMELEON_FW_COMMON_H

#include <sstream>
#include <regex>
#include <string>
#include <utility>
#include <vector>
#include "debug.hpp"

#define CHAMELEON_FW_COMMS_UDP_PORT  64000
#define CHAMELEON_FW_CMD_MAX_SIZE     9000

namespace ihd
{
    class chameleon_fw_cmd
    {
    public:
        explicit chameleon_fw_cmd(const char* cmd) : _cmd(cmd)
        {
        }

        virtual const char* to_command_string() = 0;

        virtual ~chameleon_fw_cmd() = default;

        const char* getCommand() const { return _cmd; }

    protected:
        const char* _cmd{};
        std::string _command_string;
        std::string _reply_string;
    };

    class chameleon_fw_cmd_tune : public chameleon_fw_cmd
    {
    public:
        chameleon_fw_cmd_tune(size_t c, uint64_t f, uint32_t cal) : chameleon_fw_cmd("freq_set"),
                                                                  chan(c), freq(f), cal_mask(cal)
        {
        }

        const char* to_command_string() override
        {
            std::stringstream ss;
            ss << _cmd << " chan=" << chan << "," " freq=" << freq << ", calmask=" << std::hex << cal_mask;
            _command_string = ss.str();
            return _command_string.c_str();
        }

    private:
        std::size_t chan{};
        uint64_t freq{};
        uint32_t cal_mask{};
    };

    class chameleon_fw_cmd_tune_get : public chameleon_fw_cmd
    {
    public:
        explicit chameleon_fw_cmd_tune_get(std::size_t c) : chameleon_fw_cmd("get_freq"),
                                                   chan(c)
        {
        }

        const char* to_command_string() override
        {
            std::stringstream ss;
            ss << _cmd << " chan=" << chan;
            _command_string = ss.str();
            return _command_string.c_str();
        }

    private:
        std::size_t chan{};
    };

    class chameleon_fw_cmd_txgain : public chameleon_fw_cmd
    {
    public:
        chameleon_fw_cmd_txgain(std::size_t c, double g) : chameleon_fw_cmd("set_txgain"),
                                                           chan(c), gain(g)
        {
        }

        const char* to_command_string() override
        {
            std::stringstream ss;
            ss << _cmd << " chan=" << chan << "," " gain=" << gain;
            _command_string = ss.str();
            return _command_string.c_str();
        }

    private:
        std::size_t chan{};
        double gain{};
    };

    class chameleon_fw_cmd_get_txgain : public chameleon_fw_cmd
    {
    public:
        explicit chameleon_fw_cmd_get_txgain(std::size_t c) : chameleon_fw_cmd("get_txgain"),
                                                     chan(c)
        {
        }

        const char* to_command_string() override
        {
            std::stringstream ss;
            ss << _cmd << " chan=" << chan;
            _command_string = ss.str();
            return _command_string.c_str();
        }

    private:
        std::size_t chan{};
    };

    class chameleon_fw_cmd_rxgain : public chameleon_fw_cmd
    {
    public:
        chameleon_fw_cmd_rxgain(std::size_t c, double g) : chameleon_fw_cmd("set_rxgain"),
                                                           chan(c), gain(g)
        {
        }

        const char* to_command_string() override
        {
            std::stringstream ss;
            ss << _cmd << " chan=" << chan << "," " gain=" << gain;
            _command_string = ss.str();
            return _command_string.c_str();
        }

    private:
        std::size_t chan{};
        double gain{};
    };

    class chameleon_fw_cmd_get_rxgain : public chameleon_fw_cmd
    {
    public:
        explicit chameleon_fw_cmd_get_rxgain(std::size_t c) : chameleon_fw_cmd("get_rxgain"),
                                                     chan(c)
        {
        }

        const char* to_command_string() override
        {
            std::stringstream ss;
            ss << _cmd << " chan=" << chan;
            _command_string = ss.str();
            return _command_string.c_str();
        }

    private:
        std::size_t chan{};
    };

    class chameleon_fw_stream_remove : public chameleon_fw_cmd {
    public:
        explicit chameleon_fw_stream_remove(std::uint32_t stream_id) : chameleon_fw_cmd("stream_rm"),
                    stream_id(stream_id)
        {
        }

        const char* to_command_string() override
        {
            std::stringstream ss;
            ss << _cmd << " id=" << stream_id;
            _command_string = ss.str();
            return _command_string.c_str();
        }
    private:
        std::uint32_t stream_id{};
    };

    class chameleon_fw_stream_list : public chameleon_fw_cmd {
    public:
        chameleon_fw_stream_list() : chameleon_fw_cmd("stream_list")
        {
        }

        const char* to_command_string() override
        {
            std::stringstream ss;
            ss << _cmd;
            _command_string = ss.str();
            return _command_string.c_str();
        }
    };

    class chameleon_fw_rx_cfg_set : public chameleon_fw_cmd
    {
    public:
        chameleon_fw_rx_cfg_set(const std::size_t chan, std::string data_type, const uint32_t fft_size,
                                const uint8_t avg) : chameleon_fw_cmd("rx_cfg_set"),
                                                     m_data_type(std::move(data_type)),
                                                     m_fft_size(fft_size),
                                                     m_avg(avg),
                                                     m_chan(chan)
        {
        }

        chameleon_fw_rx_cfg_set(const std::size_t chan,
                                std::string data_type, const uint16_t packet_size = 0) : chameleon_fw_cmd("rx_cfg_set"),
            m_data_type(std::move(data_type)),
            m_packet_size(packet_size),
            m_chan(chan)
        {
        }

        const char* to_command_string() override
        {
            std::stringstream ss;
            if (m_data_type == "psd")
            {
                ss << _cmd << " chan=" << m_chan << ", type=" << m_data_type << ", fft_size=" << m_fft_size << ", avg="
                    << m_avg;
            }
            else if (m_data_type == "iq")
            {
                ss << _cmd << " chan=" << m_chan << ", type=" << m_data_type << ", packet_size=" << m_packet_size;
            }
            else
            {
                dbfprintf(stderr, "Invalid stream type. Defaulting to psd");
                ss << _cmd << " chan=" << m_chan << ", type=psd, fft_size=" << m_fft_size << ", avg=" << m_avg;
            }
            _command_string = ss.str();
            return _command_string.c_str();
        }

    private:
        std::string m_data_type;
        uint32_t m_fft_size{};
        uint16_t m_avg{};
        uint16_t m_packet_size{};
        std::size_t m_chan;
    };

    class chameleon_fw_stream_rx_cfg : public chameleon_fw_cmd
    {
    public:
        chameleon_fw_stream_rx_cfg(const std::size_t chan_mask, std::string ip,
                                   const uint16_t port) : chameleon_fw_cmd("stream_rx_cfg"),
                                                          m_dest_port(port),
                                                          m_dest_ip_address(std::move(ip)),
                                                          m_chan_mask(chan_mask)
        {
        }

        const char* to_command_string() override
        {
            std::stringstream ss;
            ss << _cmd << " chan_mask=" << m_chan_mask << ", ip=" << m_dest_ip_address << ", port=" << m_dest_port;
            _command_string = ss.str();
            return _command_string.c_str();
        }

    private:
        uint16_t m_dest_port;
        std::string m_dest_ip_address;
        std::size_t m_chan_mask;
    };

    class chameleon_fw_stream_start : public chameleon_fw_cmd
    {
    public:
        explicit chameleon_fw_stream_start(const std::size_t id) : chameleon_fw_cmd("stream_start"),
                                                                   m_stream_id(id)
        {
        }

        const char* to_command_string() override
        {
            std::stringstream ss;
            ss << _cmd << " id=" << m_stream_id;
            _command_string = ss.str();
            return _command_string.c_str();
        }

    private:
        std::size_t m_stream_id;
    };

    class chameleon_fw_stream_stop : public chameleon_fw_cmd
    {
    public:
        explicit chameleon_fw_stream_stop(const std::size_t chan) : chameleon_fw_cmd("stream_stop"),
                                                                    m_chan(chan)
        {
        }

        const char* to_command_string() override
        {
            std::stringstream ss;
            ss << _cmd << " id=" << m_chan;
            _command_string = ss.str();
            return _command_string.c_str();
        }

    private:
        std::size_t m_chan;
    };

    // fpga=64.7719, trx1=63.0000, trx2=63.0000, top=52.5000, bottom=53.0000
    class chameleon_fw_get_temps_all : public chameleon_fw_cmd {
    public:
        explicit chameleon_fw_get_temps_all() : chameleon_fw_cmd("get_temps_all") {}

        const char *to_command_string() override { return _cmd; }
    };

    class chameleon_fw_cmd_stream : public chameleon_fw_cmd
    {
    public:
        explicit chameleon_fw_cmd_stream(uint32_t chan_mask, bool enable, const char* ip, uint16_t port,
                                         uint32_t size, uint32_t avg) : chameleon_fw_cmd(_cmd_str),
                                                                        _chan_mask(chan_mask), _enable(enable),
                                                                        _ip_addr(ip), _port(port),
                                                                        _fft_size(size), _avg(avg)
        {
        }

        explicit chameleon_fw_cmd_stream(bool enable, uint32_t chan_mask) : chameleon_fw_cmd(_cmd_str),
                                                                            _chan_mask(chan_mask), _enable(enable),
                                                                            _ip_addr(), _port(0),
                                                                            _fft_size(0), _avg(0)
        {
        }

        const char* to_command_string() override
        {
            std::stringstream ss;
            if (_enable)
            {
                ss << _cmd << "_start chan_mask=" << _chan_mask << ", ip=" << _ip_addr
                    << ", port=" << _port << ", fft_size="
                    << _fft_size << ", avg=" << _avg;
            }
            else
            {
                ss << _cmd << "_stop chan_mask=" << _chan_mask;
            }
            _command_string = ss.str();
            return _command_string.c_str();
        }

    private:
        constexpr static const char* const _cmd_str = "stream";
        uint32_t _chan_mask{};
        bool _enable{};
        std::string _ip_addr{};
        uint16_t _port{};
        uint32_t _fft_size{};
        uint32_t _avg{};
    };

    class chameleon_fw_comms
    {
    public:
        chameleon_fw_comms(uint32_t sequence, std::unique_ptr<chameleon_fw_cmd> command) : _sequence(sequence),
            _command(std::move(command)), _result(NONE)
        {
        }

        explicit chameleon_fw_comms(std::unique_ptr<chameleon_fw_cmd> command) : _sequence(0),
            _command(std::move(command)), _result(NONE)
        {
        }

        virtual ~chameleon_fw_comms() = default;

        [[nodiscard]]
        uint32_t getSequence() const;

        void setSequence(uint32_t sequence);

        std::string getCommandString() const;

        static std::vector<std::string> tokenize(const std::string& str, const std::regex& re);

        void setResponse(const char* response);

        void setResponseTimedOut();

        enum Result
        {
            NONE, /* No result/response yet (default value) */
            ACK,
            NAK,
            ERROR /* Invalid response, timeout, etc. */
        };

        Result getResult() const { return _result; }
        std::vector<std::string> getResponse() const { return _response; }

    private:
        uint32_t _sequence{};
        std::unique_ptr<chameleon_fw_cmd> _command{};
        Result _result;
        std::vector<std::string> _response;

        static const char* ACK_STR;
        static const char* NCK_STR;
    };
};

#endif //CHAMELEON_FW_COMMON_H
