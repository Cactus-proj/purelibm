// SPDX-License-Identifier: MIT OR Apache-2.0
#ifndef PURELIBM_TESTS_CORE_MATH_ORACLE_H
#define PURELIBM_TESTS_CORE_MATH_ORACLE_H

/* CORE-MATH consistency oracle.
 *
 * Provides helpers to compare a PureLibm function against its CORE-MATH
 * equivalent over a range of f32 bit patterns.
 *
 * Usage (M2 per-function test):
 *
 *   #include "core_math_oracle.h"
 *
 *   extern float cr_sinf(float);               // our implementation
 *   extern float cr_core_math_sinf(float);      // CORE-MATH (compiled from deps)
 *
 *   int main(void) {
 *       int mismatches = purelibm_consistency_check_f32_range(
 *           cr_sinf, cr_core_math_sinf,
 *           0x00000000u, 0xffffffffu,
 *           "sinf");
 *       return mismatches == 0 ? 0 : 1;
 *   }
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "test_common.h"
#include "utils/ieee754.h"

/* --- Consistency check over a u32 bit-pattern range ---
 *
 * For each bit pattern in [start, end], converts to float, calls both
 * our_func and core_math_func, and compares with NaN-aware equality.
 *
 * Returns the number of mismatches found (0 = perfect match).
 * Capped at 10 reported mismatches to avoid overwhelming output on
 * fundamentally broken implementations.
 */

#define PURELIBM_CONSISTENCY_MAX_REPORT 10

static inline int purelibm_consistency_check_f32_range(
    float (*our_func)(float),
    float (*core_math_func)(float),
    uint32_t start,
    uint32_t end,
    const char *label) {
    int mismatches = 0;

    /* Use uint64_t loop counter to avoid overflow when end == UINT32_MAX
     * (u++ would wrap from 0xffffffff to 0, creating an infinite loop). */
    for (uint64_t i = start; i <= (uint64_t)end; i++) {
        uint32_t u = (uint32_t)i;
        float x = purelibm_f32_from_bits(u);
        float our_y = our_func(x);
        float their_y = core_math_func(x);

        if (!purelibm_is_equal_f32(our_y, their_y)) {
            mismatches++;
            if (mismatches <= PURELIBM_CONSISTENCY_MAX_REPORT) {
                printf("CONSISTENCY FAIL [%s] x=%08" PRIx32 " (%a)"
                       " ours=%08" PRIx32 " (%a) coremath=%08" PRIx32 " (%a)\n",
                       label, u, (double)x,
                       purelibm_f32_to_bits(our_y), (double)our_y,
                       purelibm_f32_to_bits(their_y), (double)their_y);
            }
        }
    }

    if (mismatches == 0) {
        uint64_t total = (uint64_t)end - (uint64_t)start + UINT64_C(1);
        printf("CONSISTENCY OK [%s]: %" PRIu64 " inputs checked, 0 mismatches\n",
               label, total);
    } else {
        printf("CONSISTENCY FAIL [%s]: %d total mismatches\n",
               label, mismatches);
    }
    return mismatches;
}

#endif
