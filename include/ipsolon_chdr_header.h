/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/
#include <stdint.h>

#ifndef IHD_CHAMELEON_CHDR_HEADER_H
#define IHD_CHAMELEON_CHDR_HEADER_H

namespace ihd {

enum packet_type_t {
    PKT_TYPE_MGMT         = 0x0, //! Management packet
    PKT_TYPE_STRS         = 0x1, //! Stream status
    PKT_TYPE_STRC         = 0x2, //! Stream Command
    PKT_TYPE_CTRL         = 0x4, //! Control Transaction
    PKT_TYPE_DATA_NO_TS   = 0x6, //! Data Packet without TimeStamp
    PKT_TYPE_DATA_WITH_TS = 0x7, //! Data Packet with TimeStamp
};

//----------------------------------------------------
// CHDR Header
//----------------------------------------------------

class chdr_header
{
public: // Functions
    chdr_header()                       = default;
    chdr_header(const chdr_header& rhs) = default;
    chdr_header(chdr_header&& rhs)      = default;

    static constexpr size_t CHDR_W = 8;

    //! Unpack the header from a uint64_t
    explicit chdr_header(uint64_t flat_hdr) : _flat_hdr(flat_hdr) {}

    //! Get the virtual channel field (6 bits)
    inline uint8_t get_vc() const
    {
        return get_field<uint8_t>(_flat_hdr, VC_OFFSET, VC_WIDTH);
    }

    //! Set the virtual channel field (6 bits)
    inline void set_vc(uint8_t vc)
    {
        _flat_hdr = set_field(_flat_hdr, vc, VC_OFFSET, VC_WIDTH);
    }

    //! Get the end-of-burst flag (1 bit)
    inline bool get_eob() const
    {
        return get_field<bool>(_flat_hdr, EOB_OFFSET, EOB_WIDTH);
    }

    //! Set the end-of-burst flag (1 bit)
    inline void set_eob(bool eob)
    {
        _flat_hdr = set_field(_flat_hdr, eob, EOB_OFFSET, EOB_WIDTH);
    }

    //! Get the end-of-vector flag (1 bit)
    inline bool get_eov() const
    {
        return get_field<bool>(_flat_hdr, EOV_OFFSET, EOV_WIDTH);
    }

    //! Set the end-of-vector flag (1 bit)
    inline void set_eov(bool eov)
    {
        _flat_hdr = set_field(_flat_hdr, eov, EOV_OFFSET, EOV_WIDTH);
    }

    //! Get the packet type field (3 bits)
    inline packet_type_t get_pkt_type() const
    {
        return get_field<packet_type_t>(_flat_hdr, PKT_TYPE_OFFSET, PKT_TYPE_WIDTH);
    }

    //! Set the packet type field (3 bits)
    inline void set_pkt_type(packet_type_t pkt_type)
    {
        _flat_hdr = set_field(_flat_hdr, pkt_type, PKT_TYPE_OFFSET, PKT_TYPE_WIDTH);
    }

    //! Get number of metadata words field (5 bits)
    inline uint8_t get_num_mdata() const
    {
        return get_field<uint8_t>(_flat_hdr, NUM_MDATA_OFFSET, NUM_MDATA_WIDTH);
    }

    //! Set number of metadata words field (5 bits)
    inline void set_num_mdata(uint8_t num_mdata)
    {
        _flat_hdr = set_field(_flat_hdr, num_mdata, NUM_MDATA_OFFSET, NUM_MDATA_WIDTH);
    }

    //! Get the sequence number field (16 bits)
    inline uint16_t get_seq_num() const
    {
        return get_field<uint16_t>(_flat_hdr, SEQ_NUM_OFFSET, SEQ_NUM_WIDTH);
    }

    //! Set the sequence number field (16 bits)
    inline void set_seq_num(uint16_t seq_num)
    {
        _flat_hdr = set_field(_flat_hdr, seq_num, SEQ_NUM_OFFSET, SEQ_NUM_WIDTH);
    }

    //! Get the packet length field (16 bits)
    inline uint16_t get_length() const
    {
        return get_field<uint16_t>(_flat_hdr, LENGTH_OFFSET, LENGTH_WIDTH);
    }

    //! Set the packet length field (16 bits)
    inline void set_length(uint16_t length)
    {
        _flat_hdr = set_field(_flat_hdr, length, LENGTH_OFFSET, LENGTH_WIDTH);
    }

    //! Get the destination EPID field (16 bits)
    inline uint16_t get_dst_epid() const
    {
        return get_field<uint16_t>(_flat_hdr, DST_EPID_OFFSET, DST_EPID_WIDTH);
    }

    //! Set the destination EPID field (16 bits)
    inline void set_dst_epid(uint16_t dst_epid)
    {
        _flat_hdr = set_field(_flat_hdr, dst_epid, DST_EPID_OFFSET, DST_EPID_WIDTH);
    }

    //! Pack the header into a uint64_t
    inline uint64_t pack() const
    {
        return _flat_hdr;
    }

    //! Pack the header into a uint64_t as an implicit cast
    inline operator uint64_t() const
    {
        return pack();
    }

    //! Comparison operator (==)
    inline bool operator==(const chdr_header& rhs) const
    {
        return _flat_hdr == rhs._flat_hdr;
    }

    //! Comparison operator (!=)
    inline bool operator!=(const chdr_header& rhs) const
    {
        return _flat_hdr != rhs._flat_hdr;
    }

    //! Assignment operator (=) from a chdr_header
    inline const chdr_header& operator=(const chdr_header& rhs)
    {
        _flat_hdr = rhs._flat_hdr;
        return *this;
    }

    //! Assignment operator (=) from a uint64_t
    inline const chdr_header& operator=(const uint64_t& rhs)
    {
        _flat_hdr = rhs;
        return *this;
    }

    //! Return a string representation of this object
    inline const std::string to_string() const
    {
        // The static_casts are because vc and num_mdata are uint8_t -> unsigned char
        // For some reason, despite the %u meaning unsigned int, boost still formats them
        // as chars
        return str(boost::format("chdr_header{vc:%u, eob:%c, eov:%c, pkt_type:%u, "
                                 "num_mdata:%u, seq_num:%u, length:%u, dst_epid:%u}\n")
                   % static_cast<uint16_t>(get_vc()) % (get_eob() ? 'Y' : 'N')
                   % (get_eov() ? 'Y' : 'N') % get_pkt_type()
                   % static_cast<uint16_t>(get_num_mdata()) % get_seq_num() % get_length()
                   % get_dst_epid());
    }

private:
    // The flattened representation of the header stored in host order
    uint64_t _flat_hdr = 0;

    static constexpr size_t VC_WIDTH        = 6;
    static constexpr size_t EOB_WIDTH       = 1;
    static constexpr size_t EOV_WIDTH       = 1;
    static constexpr size_t PKT_TYPE_WIDTH  = 3;
    static constexpr size_t NUM_MDATA_WIDTH = 5;
    static constexpr size_t SEQ_NUM_WIDTH   = 16;
    static constexpr size_t LENGTH_WIDTH    = 16;
    static constexpr size_t DST_EPID_WIDTH  = 16;

    static constexpr size_t VC_OFFSET        = 58;
    static constexpr size_t EOB_OFFSET       = 57;
    static constexpr size_t EOV_OFFSET       = 56;
    static constexpr size_t PKT_TYPE_OFFSET  = 53;
    static constexpr size_t NUM_MDATA_OFFSET = 48;
    static constexpr size_t SEQ_NUM_OFFSET   = 32;
    static constexpr size_t LENGTH_OFFSET    = 16;
    static constexpr size_t DST_EPID_OFFSET  = 0;

    static inline uint64_t mask(const size_t width)
    {
        return ((uint64_t(1) << width) - 1);
    }

    template <typename field_t>
    static inline field_t get_field(
            const uint64_t flat_hdr, const size_t offset, const size_t width)
    {
        return static_cast<field_t>((flat_hdr >> offset) & mask(width));
    }

    template <typename field_t>
    static inline uint64_t set_field(const uint64_t old_val,
                                     const field_t field,
                                     const size_t offset,
                                     const size_t width)
    {
        return (old_val & ~(mask(width) << offset))
               | ((static_cast<uint64_t>(field) & mask(width)) << offset);
    }
};

}
#endif //IHD_CHAMELEON_CHDR_HEADER_H
