#ifndef PURELIBM_TESTS_MPFR_ORACLE_H
#define PURELIBM_TESTS_MPFR_ORACLE_H

/* Reusable MPFR oracle harness.
 *
 * Provides correctly-rounded f32/f64 reference values via MPFR so that
 * per-function exhaustive tests only need to pass a function pointer.
 *
 * Usage (M2 per-function test):
 *
 *   #include "mpfr_oracle.h"
 *
 *   static float oracle_sinf(float x) {
 *       return purelibm_mpfr_unary_f32(x, mpfr_sin, MPFR_RNDN);
 *   }
 *
 *   int main(void) {
 *       purelibm_mpfr_init();
 *       float result = oracle_sinf(0.5f);
 *       ...
 *   }
 */

#include <mpfr.h>
#include <stdint.h>

#include "utils/ieee754.h"

/* --- Initialization ---
 *
 * Call once before any oracle operations.  Sets MPFR exponent bounds to
 * encompass the full f64 subnormal range (which subsumes f32):
 *   emin = -1073  (smallest f64 subnormal is 2^-1074)
 *   emax =  1024  (largest  f64 normal    is 2^1023)
 */
static inline void purelibm_mpfr_init(void) {
    mpfr_set_emin(-1073);
    mpfr_set_emax(1024);
}

/* --- Correctly-rounded conversion from MPFR to float ---
 *
 * Rounds the MPFR value v to f32 precision using the given rounding mode,
 * then calls mpfr_subnormalize to handle subnormal results.  The ternary
 * from mpfr_set is passed to mpfr_subnormalize so MPFR can determine the
 * correct rounding direction at the subnormal / zero boundary.
 */

static inline float purelibm_mpfr_to_f32(mpfr_t v, mpfr_rnd_t rnd) {
    mpfr_t tmp;
    mpfr_init2(tmp, 24);
    int ternary = mpfr_set(tmp, v, rnd);
    (void)mpfr_subnormalize(tmp, ternary, rnd);
    float result = mpfr_get_flt(tmp, MPFR_RNDN);
    mpfr_clear(tmp);
    return result;
}

static inline double purelibm_mpfr_to_f64(mpfr_t v, mpfr_rnd_t rnd) {
    mpfr_t tmp;
    mpfr_init2(tmp, 53);
    int ternary = mpfr_set(tmp, v, rnd);
    (void)mpfr_subnormalize(tmp, ternary, rnd);
    double result = mpfr_get_d(tmp, MPFR_RNDN);
    mpfr_clear(tmp);
    return result;
}

/* --- Unary function oracle ---
 *
 * Evaluates an MPFR unary function (e.g. mpfr_sin, mpfr_exp) on a float
 * or double input and returns the correctly-rounded result.
 *
 * mpfr_func must have the standard MPFR signature:
 *   int mpfr_func(mpfr_ptr result, mpfr_srcptr input, mpfr_rnd_t rounding)
 */

typedef int (*purelibm_mpfr_unary_func)(mpfr_ptr, mpfr_srcptr, mpfr_rnd_t);

static inline float purelibm_mpfr_unary_f32(float x,
                                            purelibm_mpfr_unary_func mpfr_func,
                                            mpfr_rnd_t rnd) {
    mpfr_t y;
    mpfr_init2(y, 24);
    /* mpfr_set_flt at 24-bit precision is exact for any f32 input */
    mpfr_set_flt(y, x, MPFR_RNDN);
    int ternary = mpfr_func(y, y, rnd);
    (void)mpfr_subnormalize(y, ternary, rnd);
    float result = mpfr_get_flt(y, MPFR_RNDN);
    mpfr_clear(y);
    return result;
}

static inline double purelibm_mpfr_unary_f64(
    double x, purelibm_mpfr_unary_func mpfr_func, mpfr_rnd_t rnd) {
    mpfr_t y;
    mpfr_init2(y, 53);
    /* mpfr_set_d at 53-bit precision is exact for any f64 input */
    mpfr_set_d(y, x, MPFR_RNDN);
    int ternary = mpfr_func(y, y, rnd);
    (void)mpfr_subnormalize(y, ternary, rnd);
    double result = mpfr_get_d(y, MPFR_RNDN);
    mpfr_clear(y);
    return result;
}

#endif
