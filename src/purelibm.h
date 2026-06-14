// SPDX-License-Identifier: MIT OR Apache-2.0
#ifndef PURELIBM_H
#define PURELIBM_H

#define PURELIBM_VERSION_MAJOR 0
#define PURELIBM_VERSION_MINOR 1
#define PURELIBM_VERSION_PATCH 0

/* ============================================================
 * M2 Pilot Functions
 * ============================================================ */

/**
 * cr_expf — correctly-rounded single-precision exponential function.
 *
 * Correctness guarantee:
 *   - Correctness oracle: MPFR, f32 exhaustive test passed
 *   - Consistency oracle: CORE-MATH expf.c
 *
 * Algorithm: Range reduction via x = k*ln(2) + r, LUT for 2^k,
 * polynomial approximation for exp(r).
 *
 * Coefficient source:
 *   Sollya script: coeff/expf.sollya
 *
 * References:
 *   - CORE-MATH expf.c (deps/core-math)
 *   - Muller, "Elementary Functions", 3rd ed., Ch. 8
 */
float cr_expf(float x);

/**
 * cr_sqrtf — correctly-rounded single-precision square root.
 *
 * Correctness guarantee:
 *   - Correctness oracle: MPFR, f32 exhaustive test passed
 *
 * Algorithm: Double-precision sqrt() as starting guess, FMA-based
 * residual check, one-ulp correction if needed.
 *
 * Note: CORE-MATH has no binary32 sqrtf; this is a new implementation.
 *
 * References:
 *   - C23 §7.12.7.5 (sqrt functions)
 *   - Muller, "Elementary Functions", 3rd ed., Ch. 7
 */
float cr_sqrtf(float x);

/**
 * cr_logf — correctly-rounded single-precision natural logarithm.
 *
 * Correctness guarantee:
 *   - Correctness oracle: MPFR, f32 exhaustive test passed
 *   - Consistency oracle: CORE-MATH logf.c
 *
 * Algorithm: Range reduction via x = 2^e * m, LUT for log(m) near 1,
 * polynomial approximation for log(1+z).
 *
 * Coefficient source:
 *   Sollya script: coeff/logf.sollya
 *
 * References:
 *   - CORE-MATH logf.c (deps/core-math)
 *   - Muller, "Elementary Functions", 3rd ed., Ch. 9
 */
float cr_logf(float x);

#endif
