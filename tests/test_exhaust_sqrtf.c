// SPDX-License-Identifier: MIT OR Apache-2.0
#include "exhaustive_driver.h"
#include "test_special_values.h"

#include <stdlib.h>

extern float cr_sqrtf(float);

static float oracle_sqrtf(float x) {
    /* sqrt(-0) → -0 per C23: mpfr_sqrt(-0) produces -0 since MPFR
     * handles signed zero correctly. */
    return purelibm_mpfr_unary_f32(x, mpfr_sqrt, MPFR_RNDN);
}

/* Per-function special values for sqrtf.
 * Expected bit patterns verified against MPFR with round-to-nearest-even. */
static const purelibm_special_case_f32 sqrtf_specials[] = {
    /* sqrt(1.0) = 1.0 */
    {UINT32_C(0x3f800000), UINT32_C(0x3f800000), "sqrt(1.0) = 1.0"},
    /* sqrt(4.0) = 2.0 */
    {UINT32_C(0x40800000), UINT32_C(0x40000000), "sqrt(4.0) = 2.0"},
    /* sqrt(0.25) = 0.5 */
    {UINT32_C(0x3e800000), UINT32_C(0x3f000000), "sqrt(0.25) = 0.5"},
    /* sqrt(-1.0) = NaN */
    {UINT32_C(0xbf800000), UINT32_C(0x7fc00000), "sqrt(-1.0) = NaN"},
    /* sqrt(-Inf) = NaN */
    {UINT32_C(0xff800000), UINT32_C(0x7fc00000), "sqrt(-Inf) = NaN"},
    /* sqrt(+Inf) = +Inf */
    {UINT32_C(0x7f800000), UINT32_C(0x7f800000), "sqrt(+Inf) = +Inf"},
};

int main(int argc, char **argv) {
    uint32_t start = 0x00000000u, end = 0xffffffffu;
    if (argc == 3) {
        start = (uint32_t)strtoul(argv[1], NULL, 16);
        end   = (uint32_t)strtoul(argv[2], NULL, 16);
    }

    purelibm_mpfr_init_f32();
    int failures = 0;

    /* IEEE generic specials */
    failures += purelibm_check_specials_f32(cr_sqrtf, oracle_sqrtf, "cr_sqrtf");

    /* Per-function special values */
    failures += purelibm_check_special_table_f32(cr_sqrtf, sqrtf_specials,
        sizeof(sqrtf_specials) / sizeof(sqrtf_specials[0]), "cr_sqrtf");

    /* Exhaustive sweep (full range unless overridden by args) */
    failures += purelibm_exhaustive_run_f32(cr_sqrtf, oracle_sqrtf, "cr_sqrtf",
                                            start, end);

    return failures == 0 ? 0 : 1;
}
