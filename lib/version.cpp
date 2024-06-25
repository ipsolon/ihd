/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "version.h"
#include <sstream>

std::string ihd::get_version_string(void)
{
    std::stringstream ss;
    ss << IHD_VERSION_MAJOR << "." << IHD_VERSION_MINOR << "." << IHD_VERSION_REV << " " << __DATE__ << " " << __TIME__;
    return ss.str();
}
