// SPDX-License-Identifier: MIT OR Apache-2.0
#ifndef PURELIBM_TESTS_SPECIAL_VALUES_H
#define PURELIBM_TESTS_SPECIAL_VALUES_H

/* Per-function special-values table helper.
 *
 * Complements the generic IEEE specials check (purelibm_check_specials_f32
 * in exhaustive_driver.h) with function-specific mathematical edge cases:
 * sin(pi) ≈ 0, exp(0) = 1, sqrt(4) = 2, log(1) = 0, etc.
 *
 * Usage (per-function test):
 *
 *   static const purelibm_special_case_f32 expf_specials[] = {
 *       {UINT32_C(0x00000000), UINT32_C(0x3f800000)},  // exp(0) = 1
 *       {UINT32_C(0x42b17218), UINT32_C(...)},         // exp(88.72) ≈ max
 *   };
 *
 *   int main(void) {
 *       int f  = purelibm_check_specials_f32(cr_expf, oracle, "cr_expf");
 *       f     += purelibm_check_special_table_f32(cr_expf, expf_specials,
 *                   sizeof(expf_specials)/sizeof(expf_specials[0]), "cr_expf");
 *       ...
 *   }
 */

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "test_common.h"
#include "utils/ieee754.h"

typedef struct {
    uint32_t input_bits;     /* f32 bit pattern for input */
    uint32_t expected_bits;  /* f32 bit pattern for expected output */
    const char *description; /* human-readable label */
} purelibm_special_case_f32;

/* Evaluate a table of special cases.  Returns number of failures.
 *
 * Comparison: NaN-aware equality (both NaN = equal, signed zero distinguished).
 */
static inline int purelibm_check_special_table_f32(
    float (*f_under_test)(float),
    const purelibm_special_case_f32 *table,
    size_t table_len,
    const char *func_name) {
    int failures = 0;
    for (size_t i = 0; i < table_len; i++) {
        float x = purelibm_f32_from_bits(table[i].input_bits);
        float actual = f_under_test(x);
        float expected = purelibm_f32_from_bits(table[i].expected_bits);

        if (!purelibm_is_equal_f32(actual, expected)) {
            printf("SPECIAL-VALUE FAIL [%s] %s:"
                   " x=%08" PRIx32 " got=%08" PRIx32 " expected=%08" PRIx32 "\n",
                   func_name, table[i].description,
                   table[i].input_bits,
                   purelibm_f32_to_bits(actual),
                   table[i].expected_bits);
            failures++;
        }
    }
    return failures;
}

#endif
