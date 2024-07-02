//
// Created by jmeyers on 6/25/24.
//

#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP
#include <stdexcept>

#define THROW_NOT_IMPLEMENTED_ERROR()                \
    throw std::runtime_error(                        \
    (boost::format("function not implemented: %s\n") \
    % __FUNCTION__ ).str());

#define THROW_VALUE_NOT_SUPPORTED_ERROR(v) throw std::runtime_error("Value not supported: " + v)

#define THROW_NARROWING_ERROR() throw std::runtime_error("Narrrow error")

#endif //EXCEPTION_HPP
