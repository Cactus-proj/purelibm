#include "utils/ieee754.h"

#include <mpfr.h>
#include <stdint.h>

static int expect_f32_bits(float actual, uint32_t expected) {
    return purelibm_f32_to_bits(actual) == expected ? 0 : 1;
}

static int expect_f64_bits(double actual, uint64_t expected) {
    return purelibm_f64_to_bits(actual) == expected ? 0 : 1;
}

int main(void) {
    int failures = 0;
    mpfr_t value;

    mpfr_init2(value, 200);

    mpfr_set_d(value, 1.5, MPFR_RNDN);
    failures += expect_f32_bits(mpfr_get_flt(value, MPFR_RNDN), purelibm_f32_to_bits(1.5f));
    failures += expect_f64_bits(mpfr_get_d(value, MPFR_RNDN), purelibm_f64_to_bits(1.5));

    mpfr_set_ui_2exp(value, 1, -149, MPFR_RNDN);
    failures += expect_f32_bits(mpfr_get_flt(value, MPFR_RNDN), UINT32_C(0x00000001));

    mpfr_clear(value);
    mpfr_free_cache();

    return failures == 0 ? 0 : 1;
}
