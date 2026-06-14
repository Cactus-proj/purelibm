// SPDX-License-Identifier: MIT OR Apache-2.0
#ifndef PURELIBM_UTILS_IEEE754_H
#define PURELIBM_UTILS_IEEE754_H

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

_Static_assert(CHAR_BIT == 8, "purelibm requires 8-bit bytes");
_Static_assert(sizeof(float) == sizeof(uint32_t), "purelibm requires 32-bit float");
_Static_assert(sizeof(double) == sizeof(uint64_t), "purelibm requires 64-bit double");

#define PURELIBM_F32_SIGN_MASK UINT32_C(0x80000000)
#define PURELIBM_F32_EXP_MASK UINT32_C(0x7f800000)
#define PURELIBM_F32_MANT_MASK UINT32_C(0x007fffff)

#define PURELIBM_F64_SIGN_MASK UINT64_C(0x8000000000000000)
#define PURELIBM_F64_EXP_MASK UINT64_C(0x7ff0000000000000)
#define PURELIBM_F64_MANT_MASK UINT64_C(0x000fffffffffffff)

static inline uint32_t purelibm_f32_to_bits(float x) {
    uint32_t bits;
    memcpy(&bits, &x, sizeof(bits));
    return bits;
}

static inline float purelibm_f32_from_bits(uint32_t bits) {
    float x;
    memcpy(&x, &bits, sizeof(x));
    return x;
}

static inline uint64_t purelibm_f64_to_bits(double x) {
    uint64_t bits;
    memcpy(&bits, &x, sizeof(bits));
    return bits;
}

static inline double purelibm_f64_from_bits(uint64_t bits) {
    double x;
    memcpy(&x, &bits, sizeof(x));
    return x;
}

static inline bool purelibm_f32_sign_bit(float x) {
    return (purelibm_f32_to_bits(x) & PURELIBM_F32_SIGN_MASK) != 0;
}

static inline bool purelibm_f64_sign_bit(double x) {
    return (purelibm_f64_to_bits(x) & PURELIBM_F64_SIGN_MASK) != 0;
}

static inline uint32_t purelibm_f32_exponent_bits(float x) {
    return (purelibm_f32_to_bits(x) & PURELIBM_F32_EXP_MASK) >> 23;
}

static inline uint64_t purelibm_f64_exponent_bits(double x) {
    return (purelibm_f64_to_bits(x) & PURELIBM_F64_EXP_MASK) >> 52;
}

static inline uint32_t purelibm_f32_mantissa_bits(float x) {
    return purelibm_f32_to_bits(x) & PURELIBM_F32_MANT_MASK;
}

static inline uint64_t purelibm_f64_mantissa_bits(double x) {
    return purelibm_f64_to_bits(x) & PURELIBM_F64_MANT_MASK;
}

static inline bool purelibm_f32_is_zero(float x) {
    return (purelibm_f32_to_bits(x) & ~PURELIBM_F32_SIGN_MASK) == UINT32_C(0);
}

static inline bool purelibm_f64_is_zero(double x) {
    return (purelibm_f64_to_bits(x) & ~PURELIBM_F64_SIGN_MASK) == UINT64_C(0);
}

static inline bool purelibm_f32_is_inf(float x) {
    return (purelibm_f32_to_bits(x) & ~PURELIBM_F32_SIGN_MASK) == PURELIBM_F32_EXP_MASK;
}

static inline bool purelibm_f64_is_inf(double x) {
    return (purelibm_f64_to_bits(x) & ~PURELIBM_F64_SIGN_MASK) == PURELIBM_F64_EXP_MASK;
}

static inline bool purelibm_f32_is_nan(float x) {
    uint32_t bits = purelibm_f32_to_bits(x);
    return (bits & PURELIBM_F32_EXP_MASK) == PURELIBM_F32_EXP_MASK &&
           (bits & PURELIBM_F32_MANT_MASK) != UINT32_C(0);
}

static inline bool purelibm_f64_is_nan(double x) {
    uint64_t bits = purelibm_f64_to_bits(x);
    return (bits & PURELIBM_F64_EXP_MASK) == PURELIBM_F64_EXP_MASK &&
           (bits & PURELIBM_F64_MANT_MASK) != UINT64_C(0);
}

static inline bool purelibm_f32_is_subnormal(float x) {
    uint32_t bits = purelibm_f32_to_bits(x);
    return (bits & PURELIBM_F32_EXP_MASK) == UINT32_C(0) &&
           (bits & PURELIBM_F32_MANT_MASK) != UINT32_C(0);
}

static inline bool purelibm_f64_is_subnormal(double x) {
    uint64_t bits = purelibm_f64_to_bits(x);
    return (bits & PURELIBM_F64_EXP_MASK) == UINT64_C(0) &&
           (bits & PURELIBM_F64_MANT_MASK) != UINT64_C(0);
}

#endif
