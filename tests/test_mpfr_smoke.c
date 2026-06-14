#include "mpfr_oracle.h"
#include "test_common.h"

#include <mpfr.h>
#include <stdint.h>

/* --- Test: init sets correct exponent bounds --- */
static int test_mpfr_init_bounds(void) {
    int failures = 0;

    purelibm_mpfr_init();

    /* Verify that the smallest f32 subnormal (2^-149) can be represented */
    mpfr_t tiny;
    mpfr_init2(tiny, 24);
    mpfr_set_ui_2exp(tiny, 1, -149, MPFR_RNDN);
    /* Should not underflow — emin is -148, meaning 2^-149 is representable
     * as a subnormal */
    float result = purelibm_mpfr_to_f32(tiny, MPFR_RNDN);
    failures += purelibm_test_assert_f32_bits(result, UINT32_C(0x00000001));
    mpfr_clear(tiny);

    /* Verify that 2^127 (largest normal) can be represented */
    mpfr_t big;
    mpfr_init2(big, 24);
    mpfr_set_ui_2exp(big, 1, 127, MPFR_RNDN);
    float big_result = purelibm_mpfr_to_f32(big, MPFR_RNDN);
    failures +=
        purelibm_test_assert_f32_bits(big_result, UINT32_C(0x7f000000));
    mpfr_clear(big);

    return failures;
}

/* --- Test: f32 / f64 round-trip through MPFR --- */
static int test_mpfr_round_trip_f32(void) {
    int failures = 0;

    purelibm_mpfr_init();

    mpfr_t v;
    mpfr_init2(v, 200);

    /* Exact f32 value should round-trip perfectly */
    mpfr_set_d(v, 1.5, MPFR_RNDN);
    failures += purelibm_test_assert_f32_bits(purelibm_mpfr_to_f32(v, MPFR_RNDN),
                                              purelibm_f32_to_bits(1.5f));

    mpfr_clear(v);
    return failures;
}

static int test_mpfr_round_trip_f64(void) {
    int failures = 0;

    purelibm_mpfr_init();

    mpfr_t v;
    mpfr_init2(v, 200);

    mpfr_set_d(v, 1.5, MPFR_RNDN);
    failures += purelibm_test_assert_f64_bits(purelibm_mpfr_to_f64(v, MPFR_RNDN),
                                              purelibm_f64_to_bits(1.5));

    mpfr_clear(v);
    return failures;
}

/* --- Test: smallest f64 subnormal (2^-1074) --- */
static int test_mpfr_subnormal_f64(void) {
    int failures = 0;

    purelibm_mpfr_init();

    mpfr_t tiny;
    mpfr_init2(tiny, 53);
    mpfr_set_ui_2exp(tiny, 1, -1074, MPFR_RNDN);
    double result = purelibm_mpfr_to_f64(tiny, MPFR_RNDN);
    failures +=
        purelibm_test_assert_f64_bits(result, UINT64_C(0x0000000000000001));

    mpfr_clear(tiny);
    return failures;
}

/* --- Test: NaN-generating unary operation --- */
static int test_mpfr_unary_nan(void) {
    int failures = 0;

    purelibm_mpfr_init();

    /* sqrt(-1.0) should produce NaN */
    float result =
        purelibm_mpfr_unary_f32(-1.0f, mpfr_sqrt, MPFR_RNDN);
    failures +=
        purelibm_test_assert_true(purelibm_f32_is_nan(result));

    return failures;
}

/* --- Test: NaN-aware equality (from test_common.h) --- */
static int test_nan_equality(void) {
    int failures = 0;

    float qnan1 = purelibm_f32_from_bits(UINT32_C(0x7fc00000));
    float qnan2 = purelibm_f32_from_bits(UINT32_C(0x7fffffff));

    /* Both NaN => equal */
    failures +=
        purelibm_test_assert_true(purelibm_is_equal_f32(qnan1, qnan2));

    /* NaN vs normal => not equal */
    failures +=
        purelibm_test_assert_true(!purelibm_is_equal_f32(qnan1, 1.0f));

    return failures;
}

/* --- Test: signed zero distinguished --- */
static int test_signed_zero(void) {
    int failures = 0;

    float pos_zero = purelibm_f32_from_bits(UINT32_C(0x00000000));
    float neg_zero = purelibm_f32_from_bits(UINT32_C(0x80000000));

    failures +=
        purelibm_test_assert_true(!purelibm_is_equal_f32(pos_zero, neg_zero));

    /* Each zero equals itself */
    failures +=
        purelibm_test_assert_true(purelibm_is_equal_f32(pos_zero, pos_zero));
    failures +=
        purelibm_test_assert_true(purelibm_is_equal_f32(neg_zero, neg_zero));

    return failures;
}

/* --- Test: NaN domain error handling ---
 *
 * sin(Inf) is a domain error per C23 §7.12.4.6 and produces qNaN.
 * We test this instead of loading an sNaN bit pattern through a native
 * float (which could trigger SIGFPE on platforms with sNaN traps
 * enabled, especially under -fsignaling-nans).  The NaN classification
 * check is the same either way.
 */
static int test_domain_error_produces_nan(void) {
    int failures = 0;

    purelibm_mpfr_init();

    float inf = purelibm_f32_from_bits(UINT32_C(0x7f800000));
    float result =
        purelibm_mpfr_unary_f32(inf, mpfr_sin, MPFR_RNDN);
    failures +=
        purelibm_test_assert_true(purelibm_f32_is_nan(result));

    return failures;
}

/* --- Test: normal unary evaluation matches known value --- */
static int test_mpfr_unary_sin(void) {
    int failures = 0;

    purelibm_mpfr_init();

    /* sin(0.0) should be exactly 0.0 with correct sign */
    float r0 = purelibm_mpfr_unary_f32(0.0f, mpfr_sin, MPFR_RNDN);
    failures +=
        purelibm_test_assert_f32_bits(r0, purelibm_f32_to_bits(0.0f));

    float r0n = purelibm_mpfr_unary_f32(-0.0f, mpfr_sin, MPFR_RNDN);
    failures +=
        purelibm_test_assert_f32_bits(r0n, purelibm_f32_to_bits(-0.0f));

    return failures;
}

int main(void) {
    int failures = 0;

    failures += test_mpfr_init_bounds();
    failures += test_mpfr_round_trip_f32();
    failures += test_mpfr_round_trip_f64();
    failures += test_mpfr_subnormal_f64();
    failures += test_mpfr_unary_nan();
    failures += test_nan_equality();
    failures += test_signed_zero();
    failures += test_domain_error_produces_nan();
    failures += test_mpfr_unary_sin();

    if (failures == 0) {
        printf("mpfr_smoke: all tests PASSED\n");
    } else {
        printf("mpfr_smoke: %d assertion(s) FAILED\n", failures);
    }
    return failures == 0 ? 0 : 1;
}
