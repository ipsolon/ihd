/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/
#ifndef SAFE_MAIN_CPP_HPP
#define SAFE_MAIN_CPP_HPP

/*!
 * Defines a safe wrapper that places a catch-all around main.
 * If an exception is thrown, it prints to stderr and returns.
 * Usage: int IHD_SAFE_MAIN(int argc, char *argv[]){ main code here }
 * \param _argc the declaration for argc
 * \param _argv the declaration for argv
 */
#define IHD_SAFE_MAIN(_argc, _argv)                               \
    _main(int, char* []);                                         \
    int main(int argc, char* argv[])                              \
    {                                                             \
        try {                                                     \
            return _main(argc, argv);                             \
        } catch (const std::exception& e) {                       \
            std::cerr << "Error: " << e.what() << std::endl;      \
        } catch (...) {                                           \
            std::cerr << "Error: unknown exception" << std::endl; \
        }                                                         \
        return ~0;                                                \
    }                                                             \
    int _main(_argc, _argv)

#endif //SAFE_MAIN_CPP_HPP
