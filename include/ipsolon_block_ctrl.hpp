//
// Created by jmeyers on 11/5/24.
//

#ifndef IPSOLON_BLOCK_CTRL_HPP
#define IPSOLON_BLOCK_CTRL_HPP
#include <cstdint>
#include <string>
#include <boost/shared_ptr.hpp>

namespace ihd {

    class block_id_t {
    public:
        explicit block_id_t(const std::string &block_str) {}
        explicit block_id_t(size_t id) : _device_no(id) {}
        explicit  block_id_t() = default;
        std::string to_string() const { return _block_name; }

        //! Set from string such as "0/FFT_1", "FFT_0", ...

        //  Returns true if successful (i.e. if string valid)
        bool set(const std::string &new_name) {
            _block_name = new_name;
            return true;
        }

        //! Set the device number
        void set_device_no(size_t device_no) { _device_no = device_no; };

        //! Set the block count.
        void set_block_count(size_t count) { _block_ctr = count; };

        // Overloaded operators
        //! Assignment: Works like set(std::string)
        block_id_t operator = (const std::string &new_name) {
            set(new_name);
            return *this;
        }

        //! Return device number
        size_t get_device_no() const { return _device_no; };

        //! Return block count
        size_t get_block_count() const { return _block_ctr; };

        //! Return block name
        std::string get_block_name() const { return _block_name; };

        //! Short for to_string()
        std::string get() const { return to_string(); };

        bool operator == (const block_id_t &block_id) const {
            return (_device_no == block_id.get_device_no())
                   and (_block_name == block_id.get_block_name())
                   and (_block_ctr == block_id.get_block_count());
        }

        bool operator != (const block_id_t &block_id) const {
            return not (*this == block_id);
        }

        bool operator < (const block_id_t &block_id) const {
            return (
                    _device_no < block_id.get_device_no()
                    or (_device_no == block_id.get_device_no() and _block_name < block_id.get_block_name())
                    or (_device_no == block_id.get_device_no() and _block_name == block_id.get_block_name() and _block_ctr < block_id.get_block_count())
            );
        }

        bool operator > (const block_id_t &block_id) const {
            return (
                    _device_no > block_id.get_device_no()
                    or (_device_no == block_id.get_device_no() and _block_name > block_id.get_block_name())
                    or (_device_no == block_id.get_device_no() and _block_name == block_id.get_block_name() and _block_ctr > block_id.get_block_count())
            );
        }

        //! Check if a string matches the entire block ID (not like match())
        bool operator == (const std::string &block_id_str) const {
            return get() == block_id_str;
        }

        //! Check if a string matches the entire block ID (not like match())
        bool operator == (const char *block_id_str) const {
            std::string comp = std::string(block_id_str);
            return *this == comp;
        }

        //! Type-cast operator does the same as to_string()
        explicit operator std::string() const {
            return to_string();
        }

        //! Increment the block count ("FFT_1" -> "FFT_2")
        block_id_t operator++() {
            _block_ctr++;
            return *this;
        }

        //! Increment the block count ("FFT_1" -> "FFT_2")
        block_id_t operator++(int) {
            _block_ctr++;
            return *this;
        }

    private:
        size_t _device_no{};
        std::string _block_name{};
        size_t _block_ctr{};
    };

    class ipsolon_block_ctrl {
    protected:
        block_id_t block_id;
    public:
        explicit ipsolon_block_ctrl(uint32_t id) : block_id(id) {}
        typedef std::shared_ptr<ipsolon_block_ctrl> sptr;
        block_id_t get_block_id() const {
            return block_id;
        }
        bool overflow() {return false;} // Not implemented
        void clear_overflow() {} // Not implemented

    };

}
#endif //IPSOLON_BLOCK_CTRL_HPP
