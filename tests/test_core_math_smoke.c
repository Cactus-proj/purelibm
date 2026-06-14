/* Smoke test: verify CORE-MATH source compiles, links, and runs correctly.
 *
 * Compiles CORE-MATH's rsqrtf.c alongside this file and checks that
 * cr_rsqrtf(4.0f) ≈ 0.5f.  This is NOT a correctness test of rsqrtf — it
 * only proves the CORE-MATH build integration works end-to-end.
 */

#include "core_math_oracle.h"

#include <math.h>
#include <stdint.h>

/* CORE-MATH rsqrtf, compiled from deps/core-math/src/binary32/rsqrt/rsqrtf.c */
extern float cr_rsqrtf(float);

int main(void) {
    int failures = 0;

    /* cr_rsqrtf(4.0) should be approximately 0.5 */
    float x = 4.0f;
    float y = cr_rsqrtf(x);

    /* rsqrt(4) = 0.5 exactly in round-to-nearest; ±1 ULP tolerance
     * accounts for possible rounding differences across platforms */
    float expected = 0.5f;
    float diff = fabsf(y - expected);
    if (diff > 0x1p-23f) {
        printf("CORE-MATH SMOKE FAIL: cr_rsqrtf(4.0f) = %a (expected ~%a, diff=%a)\n",
               (double)y, (double)expected, (double)diff);
        failures++;
    }

    /* cr_rsqrtf(+Inf) should be +0 */
    float inf = purelibm_f32_from_bits(UINT32_C(0x7f800000));
    float z = cr_rsqrtf(inf);
    if (!purelibm_is_equal_f32(z, 0.0f)) {
        printf("CORE-MATH SMOKE FAIL: cr_rsqrtf(+Inf) = %a (expected +0)\n",
               (double)z);
        failures++;
    }

    if (failures == 0) {
        printf("CORE-MATH smoke: PASSED (cr_rsqrtf(4.0f) = %a)\n", (double)y);
    }
    return failures == 0 ? 0 : 1;
}
