//
// Created by jmeyers on 6/25/24.
//

#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP
#include <stdexcept>

namespace ihd
{

    /*! Base class of all UHD-specific exceptions.
     */
    struct exception : std::runtime_error
    {
        exception(const std::string& what);
        virtual unsigned code(void) const            = 0;
        virtual exception* dynamic_clone(void) const = 0;
        virtual void dynamic_throw(void) const       = 0;
    };
}

#endif //EXCEPTION_HPP
