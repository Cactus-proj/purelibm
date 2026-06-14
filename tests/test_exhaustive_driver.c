// SPDX-License-Identifier: MIT OR Apache-2.0
/* Smoke test for the exhaustive driver infrastructure.
 *
 * Exercises purelibm_check_one_f32, purelibm_exhaustive_run_f32, and
 * purelibm_check_specials_f32 with a trivial identity oracle to validate
 * that the iteration, comparison, and reporting machinery works correctly.
 */

#include "exhaustive_driver.h"

/* --- Trivial function under test: float identity --- */
static float identity_f32(float x) {
    return x;
}

/* --- MPFR-based oracle for identity ---
 *
 * We use mpfr_set (which copies a value) as the oracle.  Since f32 fits
 * exactly in 24-bit MPFR precision, the oracle returns the same bit
 * pattern as the input — except for NaN (where MPFR may change payload).
 * We therefore restrict the range to non-NaN values for this test.
 */
static float oracle_identity(float x) {
    mpfr_t y;
    mpfr_init2(y, 24);
    mpfr_set_flt(y, x, MPFR_RNDN);
    /* mpfr_set copies y←x without arithmetic, but we still round
     * through the normal pipeline to exercise it. */
    float result = mpfr_get_flt(y, MPFR_RNDN);
    mpfr_clear(y);
    return result;
}

/* --- Test: single-input check passes for a normal value --- */
static int test_check_one_normal(void) {
    int failures = 0;
    /* 1.0f should round-trip perfectly */
    failures += purelibm_check_one_f32(1.0f, identity_f32, oracle_identity,
                                       "identity");
    return failures;
}

/* --- Test: single-input check detects a deliberate mismatch --- */
static float broken_identity(float x) {
    (void)x;
    return 0.0f; /* always wrong for non-zero x */
}

static int test_check_one_mismatch(void) {
    /* purelibm_check_one_f32 should return 1 (failure) */
    int result = purelibm_check_one_f32(1.0f, broken_identity,
                                        oracle_identity, "broken");
    if (result != 1) {
        printf("FAIL: check_one did not detect broken function\n");
        return 1;
    }
    return 0;
}

/* --- Test: small-range exhaustive sweep --- */
static int test_exhaustive_small_range(void) {
    int failures = 0;

    purelibm_mpfr_init();

    /* Sweep a small range (256 values) where identity should pass */
    int result = purelibm_exhaustive_run_f32(identity_f32, oracle_identity,
                                             "identity-small",
                                             0x3f800000u, /* 1.0f */
                                             0x3f8000ffu /* 1.0f + 255 ULPs */);
    failures += result;
    return failures;
}

/* --- Test: special-values check uses all required patterns --- */
static int test_specials_coverage(void) {
    int failures = 0;

    purelibm_mpfr_init();

    /* identity should produce matching special values */
    failures += purelibm_check_specials_f32(identity_f32, oracle_identity,
                                            "identity-specials");
    return failures;
}

int main(void) {
    int failures = 0;

    purelibm_mpfr_init();

    failures += test_check_one_normal();
    failures += test_check_one_mismatch();
    failures += test_exhaustive_small_range();
    failures += test_specials_coverage();

    if (failures == 0) {
        printf("exhaustive_driver: all tests PASSED\n");
    } else {
        printf("exhaustive_driver: %d failure(s)\n", failures);
    }
    return failures == 0 ? 0 : 1;
}
