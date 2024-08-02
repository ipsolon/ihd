/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef VERSION_H
#define VERSION_H
#include <string>
#include "error.h"

#define IHD_VERSION_MAJOR 1
#define IHD_VERSION_MINOR 0
#define IHD_VERSION_REV   0

namespace ihd
{
    //! Get the version string (dotted version number)
    std::string get_version_string();
}

#endif //VERSION_H
