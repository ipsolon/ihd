//
// Created by jplaschke on 12/23/24.
//

#ifndef IHD_DEBUG_HPP
#define IHD_DEBUG_HPP
#ifndef NDEBUG
#define dbprintf(fmt, ...) \
do { printf("%s:%d:%s: " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__); } while (0)
#define dbfprintf(stream, fmt, ...) \
do { fprintf(stream, "%s:%d:%s: " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__); } while (0)

#else
#define dbprintf(fmt, ...) (void)(fmt)
#define dbfprintf(stream, fmt, ...) (void)(fmt)

#endif //NDEBUG

#endif //IHD_DEBUG_HPP
