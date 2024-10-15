/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef IHD_CHAMELEON_PACKET_H
#define IHD_CHAMELEON_PACKET_H
#include <cstdlib>

#include "chameleon_rx_stream.hpp"
#include "ipsolon_chdr_header.h"

namespace ihd {

class chameleon_packet {
public:
    explicit chameleon_packet(size_t maximum_packet_size);
    ~chameleon_packet();

    [[nodiscard]] bool endOfPacket() const;
    [[nodiscard]] uint64_t getTimestamp() const;
    [[nodiscard]] uint8_t *getPacketMem() const;
    [[nodiscard]] size_t getPacketSize() const;
    [[nodiscard]] size_t getDataSize() const;
    [[nodiscard]] size_t getPos() const;
    [[nodiscard]] chdr_header getCHDR() const;

    size_t getSamples(chameleon_rx_stream::chameleon_data_type *buff, size_t n_samples);

    void setPacketSize(size_t packetSize);
    void setPos(size_t position);
    void rewind();

private:
    size_t _packet_size;
    size_t _data_size;
    size_t _nIQ_pairs;
    size_t _pos;
    uint8_t *_packet_mem;
    int16_t *_samples;

};

} // ihd

#endif //IHD_CHAMELEON_PACKET_H
