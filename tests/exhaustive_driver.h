// SPDX-License-Identifier: MIT OR Apache-2.0
#ifndef PURELIBM_TESTS_EXHAUSTIVE_DRIVER_H
#define PURELIBM_TESTS_EXHAUSTIVE_DRIVER_H

/* Exhaustive test driver — the core of PureLibm's L1 verification.
 *
 * Provides the "doit()" pattern: for each f32 bit pattern, evaluate both
 * the function under test and the MPFR oracle, then compare with NaN-aware
 * equality.  Uses OpenMP for practical wall-clock time on 2^32 inputs.
 *
 * Usage (M2 per-function test):
 *
 *   #include "exhaustive_driver.h"
 *
 *   extern float cr_sinf(float);
 *
 *   static float oracle_sinf(float x) {
 *       return purelibm_mpfr_unary_f32(x, mpfr_sin, MPFR_RNDN);
 *   }
 *
 *   int main(void) {
 *       purelibm_mpfr_init();
 *       int f  = purelibm_check_specials_f32(cr_sinf, oracle_sinf, "cr_sinf");
 *       f     += purelibm_exhaustive_run_f32(cr_sinf, oracle_sinf, "cr_sinf",
 *                                             0x00000000u, 0xffffffffu);
 *       return f == 0 ? 0 : 1;
 *   }
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "mpfr_oracle.h"
#include "test_common.h"
#include "utils/ieee754.h"

#if defined(_OPENMP)
#include <omp.h>
#endif

/* --- Per-input check ---
 *
 * Returns 0 on match, 1 on mismatch.  Prints a diagnostic line on failure.
 */

static inline int purelibm_check_one_f32(float x,
                                         float (*f_under_test)(float),
                                         float (*f_oracle)(float),
                                         const char *func_name) {
    float y_ut = f_under_test(x);
    float y_oracle = f_oracle(x);

    if (!purelibm_is_equal_f32(y_ut, y_oracle)) {
        uint32_t xu = purelibm_f32_to_bits(x);
        uint32_t yu = purelibm_f32_to_bits(y_ut);
        uint32_t yo = purelibm_f32_to_bits(y_oracle);
        printf("EXHAUSTIVE FAIL [%s] x=%08" PRIx32 " (%a)"
               " got=%08" PRIx32 " (%a) expected=%08" PRIx32 " (%a)\n",
               func_name, xu, (double)x,
               yu, (double)y_ut,
               yo, (double)y_oracle);
        return 1;
    }
    return 0;
}

/* --- Full exhaustive sweep ---
 *
 * Iterates all f32 bit patterns in [start, end].  Uses OpenMP dynamic
 * scheduling if available.  Summary is printed after the reduction
 * barrier so checked/failures reflect global totals.
 *
 * Returns 0 if every input passes, 1 if any failure is found.
 */

#if defined(_OPENMP)
#include <omp.h>
#endif

static inline int purelibm_exhaustive_run_f32(
    float (*f_under_test)(float),
    float (*f_oracle)(float),
    const char *func_name,
    uint32_t start,
    uint32_t end) {
    uint64_t total = (uint64_t)end - (uint64_t)start + UINT64_C(1);
    uint64_t checked = 0;
    int failures = 0;

    printf("EXHAUSTIVE START [%s]: %" PRIu64 " inputs"
           " (0x%08" PRIx32 " .. 0x%08" PRIx32 ")\n",
           func_name, total, start, end);

#if defined(_OPENMP)
#pragma omp parallel
    {
#pragma omp single
        printf("EXHAUSTIVE INFO [%s]: OpenMP with %d threads\n",
               func_name, omp_get_num_threads());
    }
#endif

    /* The loop counter is uint64_t to avoid overflow when end=0xffffffff
     * (start=0, end=0xffffffff → 2^32 iterations). */
#if defined(_OPENMP)
#pragma omp parallel for schedule(dynamic, 4096) reduction(+ : failures, checked)
#endif
    for (uint64_t i = start; i <= (uint64_t)end; i++) {
        uint32_t bits = (uint32_t)i;
        float x = purelibm_f32_from_bits(bits);
        failures += purelibm_check_one_f32(x, f_under_test, f_oracle,
                                           func_name);
        checked++;
    }

    /* Progress and summary are reported after the reduction barrier so
     * checked and failures reflect the global totals from all threads. */
    if (failures == 0) {
        printf("EXHAUSTIVE PASS [%s]: %" PRIu64 " inputs checked,"
               " 0 failures\n",
               func_name, checked);
    } else {
        printf("EXHAUSTIVE FAIL [%s]: %" PRIu64 " inputs checked,"
               " %d failure(s)\n",
               func_name, checked, failures);
    }
    return failures == 0 ? 0 : 1;
}

/* --- Special-value quick-check ---
 *
 * Pre-checks ±0, ±Inf, a few quiet NaN / signaling NaN patterns, and the
 * smallest & largest subnormals.  This catches special-value bugs within
 * milliseconds so they don't waste a full exhaustive run.
 */

static inline int purelibm_check_specials_f32(
    float (*f_under_test)(float),
    float (*f_oracle)(float),
    const char *func_name) {
    int failures = 0;

    float pzero = purelibm_f32_from_bits(UINT32_C(0x00000000));
    float nzero = purelibm_f32_from_bits(UINT32_C(0x80000000));
    float pinf  = purelibm_f32_from_bits(UINT32_C(0x7f800000));
    float ninf  = purelibm_f32_from_bits(UINT32_C(0xff800000));
    float qnan1 = purelibm_f32_from_bits(UINT32_C(0x7fc00000));
    float qnan2 = purelibm_f32_from_bits(UINT32_C(0x7fffffff));
    float snan1 = purelibm_f32_from_bits(UINT32_C(0x7f800001));
    float snan2 = purelibm_f32_from_bits(UINT32_C(0x7fbfffff));
    float ssub  = purelibm_f32_from_bits(UINT32_C(0x00000001));
    float lsub  = purelibm_f32_from_bits(UINT32_C(0x007fffff));

    failures += purelibm_check_one_f32(pzero, f_under_test, f_oracle, func_name);
    failures += purelibm_check_one_f32(nzero, f_under_test, f_oracle, func_name);
    failures += purelibm_check_one_f32(pinf,  f_under_test, f_oracle, func_name);
    failures += purelibm_check_one_f32(ninf,  f_under_test, f_oracle, func_name);
    failures += purelibm_check_one_f32(qnan1, f_under_test, f_oracle, func_name);
    failures += purelibm_check_one_f32(qnan2, f_under_test, f_oracle, func_name);
    failures += purelibm_check_one_f32(snan1, f_under_test, f_oracle, func_name);
    failures += purelibm_check_one_f32(snan2, f_under_test, f_oracle, func_name);
    failures += purelibm_check_one_f32(ssub,  f_under_test, f_oracle, func_name);
    failures += purelibm_check_one_f32(lsub,  f_under_test, f_oracle, func_name);

    return failures;
}

/* ================================================================
 * f64 variants — same API as f32, for double-precision functions.
 *
 * Unlike f32, a full 2^64 exhaustive sweep is infeasible.  These
 * functions are designed for subrange scans, random sampling, and
 * boundary-value checks as specified in DESIGN.md §4.4.
 * ================================================================ */

static inline int purelibm_check_one_f64(double x,
                                         double (*f_under_test)(double),
                                         double (*f_oracle)(double),
                                         const char *func_name) {
    double y_ut = f_under_test(x);
    double y_oracle = f_oracle(x);

    if (!purelibm_is_equal_f64(y_ut, y_oracle)) {
        uint64_t xu = purelibm_f64_to_bits(x);
        uint64_t yu = purelibm_f64_to_bits(y_ut);
        uint64_t yo = purelibm_f64_to_bits(y_oracle);
        printf("EXHAUSTIVE FAIL [%s] x=%016" PRIx64 " (%a)"
               " got=%016" PRIx64 " (%a) expected=%016" PRIx64 " (%a)\n",
               func_name, xu, x,
               yu, y_ut,
               yo, y_oracle);
        return 1;
    }
    return 0;
}

static inline int purelibm_exhaustive_run_f64(
    double (*f_under_test)(double),
    double (*f_oracle)(double),
    const char *func_name,
    uint64_t start,
    uint64_t end) {
    uint64_t total = (end - start) + UINT64_C(1);
    if (end < start) total = UINT64_C(0);
    uint64_t checked = 0;
    int failures = 0;

    printf("EXHAUSTIVE START [%s]: %" PRIu64 " inputs"
           " (0x%016" PRIx64 " .. 0x%016" PRIx64 ")\n",
           func_name, total, start, end);

    for (uint64_t i = start; i <= end && i >= start; i++) {
        double x = purelibm_f64_from_bits(i);
        failures += purelibm_check_one_f64(x, f_under_test, f_oracle,
                                           func_name);
        checked++;
    }

    if (failures == 0) {
        printf("EXHAUSTIVE PASS [%s]: %" PRIu64 " inputs checked,"
               " 0 failures\n",
               func_name, checked);
    } else {
        printf("EXHAUSTIVE FAIL [%s]: %" PRIu64 " inputs checked,"
               " %d failure(s)\n",
               func_name, checked, failures);
    }
    return failures == 0 ? 0 : 1;
}

static inline int purelibm_check_specials_f64(
    double (*f_under_test)(double),
    double (*f_oracle)(double),
    const char *func_name) {
    int failures = 0;

    double pzero = purelibm_f64_from_bits(UINT64_C(0x0000000000000000));
    double nzero = purelibm_f64_from_bits(UINT64_C(0x8000000000000000));
    double pinf  = purelibm_f64_from_bits(UINT64_C(0x7ff0000000000000));
    double ninf  = purelibm_f64_from_bits(UINT64_C(0xfff0000000000000));
    double qnan1 = purelibm_f64_from_bits(UINT64_C(0x7ff8000000000000));
    double qnan2 = purelibm_f64_from_bits(UINT64_C(0x7fffffffffffffff));
    double snan1 = purelibm_f64_from_bits(UINT64_C(0x7ff0000000000001));
    double snan2 = purelibm_f64_from_bits(UINT64_C(0x7ff7ffffffffffff));
    double ssub  = purelibm_f64_from_bits(UINT64_C(0x0000000000000001));
    double lsub  = purelibm_f64_from_bits(UINT64_C(0x000fffffffffffff));

    failures += purelibm_check_one_f64(pzero, f_under_test, f_oracle, func_name);
    failures += purelibm_check_one_f64(nzero, f_under_test, f_oracle, func_name);
    failures += purelibm_check_one_f64(pinf,  f_under_test, f_oracle, func_name);
    failures += purelibm_check_one_f64(ninf,  f_under_test, f_oracle, func_name);
    failures += purelibm_check_one_f64(qnan1, f_under_test, f_oracle, func_name);
    failures += purelibm_check_one_f64(qnan2, f_under_test, f_oracle, func_name);
    failures += purelibm_check_one_f64(snan1, f_under_test, f_oracle, func_name);
    failures += purelibm_check_one_f64(snan2, f_under_test, f_oracle, func_name);
    failures += purelibm_check_one_f64(ssub,  f_under_test, f_oracle, func_name);
    failures += purelibm_check_one_f64(lsub,  f_under_test, f_oracle, func_name);

    return failures;
}

#endif
