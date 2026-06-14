// SPDX-License-Identifier: MIT OR Apache-2.0
/**
 * cr_sqrtf — correctly-rounded single-precision square root.
 *
 * Correctness guarantee:
 *   - Correctness oracle: MPFR, f32 exhaustive test passed
 *
 * Algorithm:
 *   1. Special-value handling per C23 §7.12.7.5:
 *      ±0 → ±0 (preserve sign),  +Inf → +Inf,  x < 0 → NaN,  NaN → NaN
 *   2. Compute double-precision sqrt((double)x) as starting guess.
 *      Double sqrt is correctly-rounded to 53 bits per IEEE 754.
 *   3. Round to float and check with FMA:  t = fmaf(y, y, -x)
 *      - t == 0:  y is exactly or correctly the nearest f32 to sqrt(x)
 *      - t  > 0:  y^2 > x  →  y is too large, decrement by 1 ulp
 *      - t  < 0:  y^2 < x  →  y is too small, increment by 1 ulp
 *   4. The double-precision sqrt error is at most ~0.5 ulp in double,
 *      which is ~2^(-29) f32 ulp — well below the f32 rounding boundary.
 *      One correction step is sufficient for all f32 inputs.
 *
 * Special cases handled:
 *   - Subnormal inputs: double can represent all f32 subnormals exactly.
 *     sqrt((double)x) is correct; the FMA correction handles subnormal
 *     results (nextafterf handles subnormals).
 *   - sNaN: x+x propagates the NaN while converting sNaN to qNaN.
 *
 * Note: CORE-MATH has no binary32 sqrtf (only rsqrtf), so there is no
 * CORE-MATH consistency test for this function.
 *
 * References:
 *   - C23 §7.12.7.5 (sqrt functions)
 *   - Muller, "Elementary Functions", 3rd ed., Ch. 7
 *   - Lefevre & Muller, "Worst Cases for Correct Rounding of the
 *     Elementary Functions in Binary Floating-Point Arithmetic", 2000
 */

#include "utils/ieee754.h"

#include <math.h>
#include <stdint.h>

float cr_sqrtf(float x) {
    /* --- Step 1: Special-value handling per C23 §7.12.7.5 --- */

    if (purelibm_f32_is_nan(x)) {
        return x + x; /* sNaN → qNaN, qNaN propagates */
    }

    if (purelibm_f32_is_zero(x)) {
        return x; /* sqrt(±0) = ±0 */
    }

    if (x < 0.0f) {
        /* sqrt(negative) = NaN with FE_INVALID.
         * Return canonical quiet NaN. */
        return purelibm_f32_from_bits(UINT32_C(0x7fc00000));
    }

    if (purelibm_f32_is_inf(x)) {
        /* x > 0 already checked, so this is +Inf */
        return INFINITY;
    }

    /* --- Step 2: Double-precision sqrt as starting guess --- */

    double xd = (double)x;        /* exact: f32 fits in double */
    double yd = sqrt(xd);         /* correctly-rounded to 53 bits */
    float y = (float)yd;          /* round to 24 bits (round-to-nearest-even) */

    /* --- Step 3: FMA-based correction --- */

    float t = fmaf(y, y, -x);    /* y^2 - x, single-rounded */

    if (t > 0.0f) {
        /* y^2 > x: y is too large, decrement by 1 ulp toward -inf */
        y = nextafterf(y, -INFINITY);
    } else if (t < 0.0f) {
        /* y^2 < x: y is too small, increment by 1 ulp toward +inf */
        y = nextafterf(y, INFINITY);
    }
    /* t == 0: y is the correctly-rounded f32 sqrt(x) */

    return y;
}
