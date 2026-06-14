// SPDX-License-Identifier: MIT OR Apache-2.0
#include "core_math_oracle.h"

/* PureLibm implementation */
extern float cr_expf(float);

/* CORE-MATH implementation (renamed via -D cr_expf=core_math_expf) */
extern float core_math_expf(float);

int main(void) {
    int mismatches = purelibm_consistency_check_f32_range(
        cr_expf, core_math_expf,
        0x00000000u, 0xffffffffu,
        "expf");
    return mismatches == 0 ? 0 : 1;
}
