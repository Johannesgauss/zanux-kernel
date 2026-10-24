#ifndef UTIL_ALIAS_H
#define UTIL_ALIAS_H
#include <stdint.h>
typedef int8_t byte;
typedef uint8_t ubyte;

typedef int8_t tiny;
typedef uint8_t utiny;

typedef unsigned short ushort;
typedef unsigned long long int ullint;
typedef long long int llint;

typedef unsigned int uint;

typedef long long int max;
typedef unsigned long long int umax;

#define global static
#define intern static
#define local static
typedef uint64_t addr;
typedef uint64_t phy_t;
typedef void *virt_t;

typedef uint64_t* virt_array;
//typedef unsigned long long int size_t;

#endif // UTIL_ALIAS_H
