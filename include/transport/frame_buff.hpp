/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef FRAME_BUFF_HPP
#define FRAME_BUFF_HPP

#include <memory>

namespace ihd::transport {

/*!
 * Contains a reference to a frame buffer managed by a link.
 */
class frame_buff
{
public:
    /*!
     * No-op deleter to prevent unique_ptr from deleting the buffer if the
     * pointer goes out of scope. The lifetime of the buffers is controlled
     * by the links.
     */
    struct deleter
    {
        void operator()(frame_buff*) {}
    };

    using uptr = std::unique_ptr<frame_buff, deleter>;

    /*!
     * Get the raw data buffer contained within the frame buffer
     * \return a pointer to the buffer memory.
     */
    [[nodiscard]] void* data() const
    {
        return _data;
    }

    /*!
     * Returns the size of the packet
     * \return the size of the packet contained in the frame buffer, in bytes.
     */
    [[nodiscard]] size_t packet_size() const
    {
        return _packet_size;
    }

    /*!
     * Sets the size of the packet contained in the frame buffer, in bytes.
     * \param size Number of bytes occupied in the buffer
     */
    void set_packet_size(size_t size)
    {
        _packet_size = size;
    }

protected:
    /*! Pointer to data of current frame */
    void* _data = nullptr;

    /*! Size of packet in current frame */
    size_t _packet_size = 0;
};

} // namespace uhd::transport

#endif // FRAME_BUFF_HPP