// SPDX-License-Identifier: MIT OR Apache-2.0
#include "core_math_oracle.h"

/* PureLibm implementation */
extern float cr_logf(float);

/* CORE-MATH implementation (renamed via -D cr_logf=core_math_logf) */
extern float core_math_logf(float);

int main(void) {
    int mismatches = purelibm_consistency_check_f32_range(
        cr_logf, core_math_logf,
        0x00000000u, 0xffffffffu,
        "logf");
    return mismatches == 0 ? 0 : 1;
}
