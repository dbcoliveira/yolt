#ifndef _UTILS_H
#define _UTILS_H

#define check_bit(var, n) (((var) & (1<<(n))) ? true : false)
#define set_bit(var, n, b) (var & ~(1<<n)) | ((b << n) & (1<<n))

typedef enum {false, true} bool;

#define NULL ((void *)0)

// x86_64

typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned int u32_t;
typedef unsigned long u64_t;

#endif