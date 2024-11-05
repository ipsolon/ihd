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
        explicit block_id_t(uint32_t id) : block_id(id) {}
        std::string to_string() const { return std::to_string(block_id); }
    private:
        uint32_t block_id;
    };

    class ipsolon_block_ctrl {
    protected:
        block_id_t block_id;
    public:
        explicit ipsolon_block_ctrl(uint32_t id) : block_id(id) {}
        typedef std::shared_ptr<ipsolon_block_ctrl> sptr;
        const block_id_t &get_block_id() const {
            return block_id;
        }
    };

}
#endif //IPSOLON_BLOCK_CTRL_HPP
