/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef ERROR_H
#define ERROR_H

//! IHD error codes
/*!
 * Each error code corresponds to a specific ihd::exception, with
 * extra codes corresponding to a boost::exception, std::exception,
 * and a catch-all for everything else. When an internal C++ function
 * throws an exception, IHD converts it to one of these error codes
 * to return on the C level.
 */
typedef enum {

    //! No error thrown.
    IHD_ERROR_NONE = 0,
    //! Invalid device arguments.
    IHD_ERROR_INVALID_DEVICE = 1,

    //! See ihd::index_error.
    IHD_ERROR_INDEX = 10,
    //! See ihd::key_error.
    IHD_ERROR_KEY = 11,

    //! See ihd::not_implemented_error.
    IHD_ERROR_NOT_IMPLEMENTED = 20,
    //! See ihd::usb_error.
    IHD_ERROR_USB = 21,

    //! See ihd::io_error.
    IHD_ERROR_IO = 30,
    //! See ihd::os_error.
    IHD_ERROR_OS = 31,

    //! See ihd::assertion_error.
    IHD_ERROR_ASSERTION = 40,
    //! See ihd::lookup_error.
    IHD_ERROR_LOOKUP = 41,
    //! See ihd::type_error.
    IHD_ERROR_TYPE = 42,
    //! See ihd::value_error.
    IHD_ERROR_VALUE = 43,
    //! See ihd::runtime_error.
    IHD_ERROR_RUNTIME = 44,
    //! See ihd::environment_error.
    IHD_ERROR_ENVIRONMENT = 45,
    //! See ihd::system_error.
    IHD_ERROR_SYSTEM = 46,
    //! See ihd::exception.
    IHD_ERROR_EXCEPT = 47,

    //! A boost::exception was thrown.
    IHD_ERROR_BOOSTEXCEPT = 60,

    //! A std::exception was thrown.
    IHD_ERROR_STDEXCEPT = 70,

    //! An unknown error was thrown.
    IHD_ERROR_UNKNOWN = 100
} ihd_error;

#endif //ERROR_H
