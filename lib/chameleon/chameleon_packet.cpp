/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "chameleon_packet.hpp"
#include "chameleon_stream.hpp"

using namespace ihd;

chameleon_packet::chameleon_packet(size_t maximum_packet_size) :_packet_size(maximum_packet_size),
                                                                _data_size(maximum_packet_size - ipsolon_stream::PACKET_HEADER_SIZE),
                                                                _nIQ_pairs(_data_size / ipsolon_stream::BYTES_PER_IQ_PAIR),
                                                                _pos(0),
                                                                _samples(nullptr)
{
    _packet_mem = static_cast<uint8_t *>(malloc(_packet_size));
    if(_packet_mem == nullptr) {
        THROW_MALLOC_ERROR();
    } else {
        _samples = reinterpret_cast<int16_t *>(_packet_mem + ipsolon_stream::PACKET_HEADER_SIZE);
    }
}

chameleon_packet::~chameleon_packet()
{
    if (_packet_mem != nullptr) {
        free(_packet_mem);
    }
}

[[nodiscard]]
chdr_header chameleon_packet::getCHDR() const
{
    if (_packet_mem == nullptr) {
        return chdr_header(0);
    } else {
        return chdr_header(
                (uint64_t) _packet_mem[0] |
                (uint64_t) _packet_mem[1] << 8 |
                (uint64_t) _packet_mem[2] << 16 |
                (uint64_t) _packet_mem[3] << 24 |
                (uint64_t) _packet_mem[4] << 32 |
                (uint64_t) _packet_mem[5] << 40 |
                (uint64_t) _packet_mem[6] << 48 |
                (uint64_t) _packet_mem[7] << 56);
    }
}

[[nodiscard]]
uint64_t chameleon_packet::getTimestamp() const
{
    if(_packet_mem == nullptr) {
        return 0;
    } else {
        return (uint64_t) _packet_mem[0] |
               (uint64_t) _packet_mem[1] << 8 |
               (uint64_t) _packet_mem[2] << 16 |
               (uint64_t) _packet_mem[3] << 24 |
               (uint64_t) _packet_mem[4] << 32 |
               (uint64_t) _packet_mem[5] << 40 |
               (uint64_t) _packet_mem[6] << 48 |
               (uint64_t) _packet_mem[7] << 56;;
    }
}

[[nodiscard]]
size_t chameleon_packet::getPacketSize() const
{
    return _packet_size;
}

void chameleon_packet::setPacketSize(size_t packetSize)
{
    _packet_size = packetSize;
    _data_size = _packet_size - ipsolon_stream::PACKET_HEADER_SIZE;
    _nIQ_pairs = _data_size / ipsolon_stream::BYTES_PER_IQ_PAIR;
    rewind();
}

[[nodiscard]]
size_t chameleon_packet::getDataSize() const
{
    return _data_size;
}

size_t chameleon_packet::getPos() const
{
    return _pos;
}

void chameleon_packet::setPos(size_t position)
{
    _pos = std::min(position, _data_size - 1);
}

void chameleon_packet::rewind()
{
    setPos(0);
}

size_t chameleon_packet::getSamples(chameleon_stream::chameleon_data_type *buff, size_t n_samples)
{
    size_t n = std::min(n_samples, _nIQ_pairs - _pos);
    for (size_t i = 0; i < n; i++) {
        size_t s = (i * 2);
        buff[i] = chameleon_stream::chameleon_data_type(_samples[s], _samples[s + 1]);
    }
    _pos += n; // Move position in packet
    return n;
}

[[nodiscard]]
bool chameleon_packet::endOfPacket() const
{
    return (_pos >= _nIQ_pairs);
}

[[nodiscard]]
uint8_t *chameleon_packet::getPacketMem() const
{
    return _packet_mem;
}
