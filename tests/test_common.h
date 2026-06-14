// SPDX-License-Identifier: MIT OR Apache-2.0
#ifndef PURELIBM_TESTS_COMMON_H
#define PURELIBM_TESTS_COMMON_H

#include "utils/ieee754.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* --- Assertion helpers ---
 *
 * Each returns 0 on pass, 1 on failure (accumulable into a failure counter).
 */

static inline int purelibm_test_assert_true(bool value) {
    return value ? 0 : 1;
}

static inline int purelibm_test_assert_u32(uint32_t actual, uint32_t expected) {
    return actual == expected ? 0 : 1;
}

static inline int purelibm_test_assert_u64(uint64_t actual, uint64_t expected) {
    return actual == expected ? 0 : 1;
}

static inline int purelibm_test_assert_f32_bits(float actual,
                                                uint32_t expected) {
    return purelibm_f32_to_bits(actual) == expected ? 0 : 1;
}

static inline int purelibm_test_assert_f64_bits(double actual,
                                                uint64_t expected) {
    return purelibm_f64_to_bits(actual) == expected ? 0 : 1;
}

/* --- NaN-aware equality ---
 *
 * DESIGN.md §4.3: NaN default only requires classification equality
 * (both sides NaN => equal, regardless of payload). Signed zero must be
 * bitwise distinguished (+0.0 ≠ -0.0).
 */

static inline bool purelibm_is_equal_f32(float a, float b) {
    if (purelibm_f32_is_nan(a))
        return purelibm_f32_is_nan(b);
    if (purelibm_f32_is_nan(b))
        return false;
    return purelibm_f32_to_bits(a) == purelibm_f32_to_bits(b);
}

static inline bool purelibm_is_equal_f64(double a, double b) {
    if (purelibm_f64_is_nan(a))
        return purelibm_f64_is_nan(b);
    if (purelibm_f64_is_nan(b))
        return false;
    return purelibm_f64_to_bits(a) == purelibm_f64_to_bits(b);
}

/* --- NaN-aware assertion --- */

static inline int purelibm_test_assert_equal_f32(float actual,
                                                  float expected) {
    return purelibm_is_equal_f32(actual, expected) ? 0 : 1;
}

static inline int purelibm_test_assert_equal_f64(double actual,
                                                  double expected) {
    return purelibm_is_equal_f64(actual, expected) ? 0 : 1;
}

#endif
