// SPDX-License-Identifier: MIT OR Apache-2.0
#include "exhaustive_driver.h"
#include "test_special_values.h"

#include <stdlib.h>

extern float cr_expf(float);

static float oracle_expf(float x) {
    return purelibm_mpfr_unary_f32(x, mpfr_exp, MPFR_RNDN);
}

/* Per-function special values for expf.
 *
 * Expected bit patterns verified against MPFR with round-to-nearest-even.
 */
static const purelibm_special_case_f32 expf_specials[] = {
    /* exp(+0) = 1.0 */
    {UINT32_C(0x00000000), UINT32_C(0x3f800000), "exp(+0) = 1.0"},
    /* exp(-0) = 1.0 */
    {UINT32_C(0x80000000), UINT32_C(0x3f800000), "exp(-0) = 1.0"},
    /* exp(1.0) = e */
    {UINT32_C(0x3f800000), UINT32_C(0x402df854), "exp(1.0) = e"},
    /* exp(-inf) = +0 */
    {UINT32_C(0xff800000), UINT32_C(0x00000000), "exp(-Inf) = +0"},
    /* exp(+inf) = +inf */
    {UINT32_C(0x7f800000), UINT32_C(0x7f800000), "exp(+Inf) = +Inf"},
    /* exp(NaN) = NaN */
    {UINT32_C(0x7fc00000), UINT32_C(0x7fc00000), "exp(qNaN) = NaN"},
    /* exp(large positive) overflows to +Inf */
    {UINT32_C(0x42b17218), UINT32_C(0x7f800000), "exp(> 0x1.62e42ep+6) = +Inf"},
};

int main(int argc, char **argv) {
    uint32_t start = 0x00000000u, end = 0xffffffffu;
    if (argc == 3) {
        start = (uint32_t)strtoul(argv[1], NULL, 16);
        end   = (uint32_t)strtoul(argv[2], NULL, 16);
    }

    purelibm_mpfr_init();
    int failures = 0;

    /* IEEE generic specials */
    failures += purelibm_check_specials_f32(cr_expf, oracle_expf, "cr_expf");

    /* Per-function special values */
    failures += purelibm_check_special_table_f32(cr_expf, expf_specials,
        sizeof(expf_specials) / sizeof(expf_specials[0]), "cr_expf");

    /* Exhaustive sweep (full range unless overridden by args) */
    failures += purelibm_exhaustive_run_f32(cr_expf, oracle_expf, "cr_expf",
                                            start, end);

    return failures == 0 ? 0 : 1;
}
