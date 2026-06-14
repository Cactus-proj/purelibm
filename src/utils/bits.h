// SPDX-License-Identifier: MIT OR Apache-2.0
#ifndef PURELIBM_UTILS_BITS_H
#define PURELIBM_UTILS_BITS_H

/* Portable bit-manipulation utilities.
 *
 * These replace compiler builtins (__builtin_clz, __builtin_ctz, etc.)
 * with standard C implementations suitable for the -Wpedantic,
 * no-GNU-extensions build contract established in ADR 001.
 */

#include <stdint.h>

/* Count leading zeros for 32-bit unsigned integer.
 * Returns 32 for input 0 (matches __builtin_clz behaviour on x=0
 * for most architectures, though the builtin is technically undefined
 * for 0). */
static inline int purelibm_clz32(uint32_t x) {
    if (x == 0)
        return 32;
    int n = 0;
    if ((x & 0xFFFF0000u) == 0) { n += 16; x <<= 16; }
    if ((x & 0xFF000000u) == 0) { n += 8;  x <<= 8;  }
    if ((x & 0xF0000000u) == 0) { n += 4;  x <<= 4;  }
    if ((x & 0xC0000000u) == 0) { n += 2;  x <<= 2;  }
    if ((x & 0x80000000u) == 0) { n += 1;             }
    return n;
}

#endif
