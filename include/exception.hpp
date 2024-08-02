/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP
#include <stdexcept>

#define THROW_NOT_IMPLEMENTED_ERROR()                \
    throw std::runtime_error(                        \
    (boost::format("function not implemented: %s\n") \
    % __FUNCTION__ ).str());

#define THROW_VALUE_NOT_SUPPORTED_ERROR(v) throw std::runtime_error("Value not supported: " + v)

#define THROW_SOCKET_ERROR() throw std::runtime_error("UDP Socket error")

#define THROW_TYPE_ERROR() throw std::runtime_error("Type cast error")

#define THROW_MALLOC_ERROR() throw std::runtime_error("Memory Allocation error")

#endif //EXCEPTION_HPP
