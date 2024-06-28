/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/
#include <iostream>
#include <uhd/transport/udp_simple.hpp>
#include "chameleon_fw_common.h"

#include "chameleon_stream.hpp"
#include "exception.hpp"

using namespace ihd;

chameleon_stream::chameleon_stream(const uhd::stream_args_t& stream_cmd, const uhd::device_addr_t& device_addr) :
    _nChans(stream_cmd.channels.size()), _commander(device_addr), _chanMask(0)
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
}

size_t chameleon_stream::get_num_channels() const
{
    return _nChans;
}

size_t chameleon_stream::get_max_num_samps() const
{
    return max_sample_per_packet;
}

size_t chameleon_stream::recv(const buffs_type& buffs, const size_t nsamps_per_buff, uhd::rx_metadata_t& metadata,
    const double timeout, const bool one_packet)
{
    THROW_NOT_IMPLEMENTED_ERROR();
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

void chameleon_stream::start_stream() const
{
    auto request = chameleon_fw_comms_t();
    request.flags             =  CHAMELEON_FW_COMMS_FLAGS_WRITE;
    request.addr              = CHAMELEON_FW_COMMS_CMD_STREAM_CMD;
    request.stream.enable     = true;
    request.stream.chans      = _chanMask;

    _commander.send_request(request);
}

void chameleon_stream::stop_stream() const
{
    auto request = chameleon_fw_comms_t();
    request.flags             = CHAMELEON_FW_COMMS_FLAGS_WRITE;
    request.addr              = CHAMELEON_FW_COMMS_CMD_STREAM_CMD;
    request.stream.enable     = false;
    request.stream.chans      = _chanMask;

    _commander.send_request(request);
}
