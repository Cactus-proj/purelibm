// SPDX-License-Identifier: MIT OR Apache-2.0
#include "test_common.h"
#include "exhaustive_harness.h"
#include "utils/ieee754.h"

#include <stdint.h>

int main(void) {
    int failures = 0;
    uint32_t bits = UINT32_C(0);
    uint64_t seen = UINT64_C(0);
    purelibm_u32_range range =
        purelibm_u32_range_make(UINT32_C(0x00000000), UINT32_C(0x00000003));

    while (purelibm_u32_range_next(&range, &bits)) {
        float x = purelibm_f32_from_bits(bits);
        failures +=
            purelibm_test_assert_u32(purelibm_f32_to_bits(x), bits);
        seen += UINT64_C(1);
    }

    failures += purelibm_test_assert_u64(seen, UINT64_C(4));
    failures += purelibm_test_assert_u64(
        purelibm_count_u32_range(UINT32_C(0x00000000), UINT32_C(0x00000003)),
        UINT64_C(4));
    failures += purelibm_test_assert_u64(
        purelibm_count_u32_range(UINT32_C(0x00000003), UINT32_C(0x00000000)),
        UINT64_C(0));

    purelibm_u32_range empty =
        purelibm_u32_range_make(UINT32_C(3), UINT32_C(0));
    failures +=
        purelibm_test_assert_true(!purelibm_u32_range_next(&empty, &bits));

    return failures == 0 ? 0 : 1;
}
