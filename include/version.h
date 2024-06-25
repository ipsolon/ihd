//
// Created by jmeyers on 6/25/24.
//

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
