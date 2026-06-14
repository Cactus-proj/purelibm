// SPDX-License-Identifier: MIT OR Apache-2.0
#include "exhaustive_driver.h"
#include "test_special_values.h"

#include <stdlib.h>

extern float cr_logf(float);

static float oracle_logf(float x) {
    return purelibm_mpfr_unary_f32(x, mpfr_log, MPFR_RNDN);
}

/* Per-function special values for logf.
 * Expected bit patterns verified against MPFR with round-to-nearest-even. */
static const purelibm_special_case_f32 logf_specials[] = {
    /* log(1.0) = 0.0 */
    {UINT32_C(0x3f800000), UINT32_C(0x00000000), "log(1.0) = 0.0"},
    /* log(+Inf) = +Inf */
    {UINT32_C(0x7f800000), UINT32_C(0x7f800000), "log(+Inf) = +Inf"},
    /* log(-1.0) = NaN (domain error) */
    {UINT32_C(0xbf800000), UINT32_C(0x7fc00000), "log(-1.0) = NaN"},
    /* log(2.0) = ln(2) */
    {UINT32_C(0x40000000), UINT32_C(0x3f317218), "log(2.0) = ln(2)"},
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
    failures += purelibm_check_specials_f32(cr_logf, oracle_logf, "cr_logf");

    /* Per-function special values */
    failures += purelibm_check_special_table_f32(cr_logf, logf_specials,
        sizeof(logf_specials) / sizeof(logf_specials[0]), "cr_logf");

    /* Exhaustive sweep (full range unless overridden by args) */
    failures += purelibm_exhaustive_run_f32(cr_logf, oracle_logf, "cr_logf",
                                            start, end);

    return failures == 0 ? 0 : 1;
}
