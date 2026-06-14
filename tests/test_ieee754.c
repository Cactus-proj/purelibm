#include "test_common.h"
#include "utils/ieee754.h"

#include <stdint.h>

int main(void) {
    int failures = 0;

    float pos_zero_f32 = purelibm_f32_from_bits(UINT32_C(0x00000000));
    float neg_zero_f32 = purelibm_f32_from_bits(UINT32_C(0x80000000));
    float pos_inf_f32 = purelibm_f32_from_bits(UINT32_C(0x7f800000));
    float nan_f32 = purelibm_f32_from_bits(UINT32_C(0x7fc00001));
    float subnormal_f32 = purelibm_f32_from_bits(UINT32_C(0x00000001));

    double pos_zero_f64 = purelibm_f64_from_bits(UINT64_C(0x0000000000000000));
    double neg_zero_f64 = purelibm_f64_from_bits(UINT64_C(0x8000000000000000));
    double neg_inf_f64 = purelibm_f64_from_bits(UINT64_C(0xfff0000000000000));
    double nan_f64 = purelibm_f64_from_bits(UINT64_C(0x7ff8000000000001));
    double subnormal_f64 = purelibm_f64_from_bits(UINT64_C(0x0000000000000001));

    failures += purelibm_test_assert_true(purelibm_f32_is_zero(pos_zero_f32));
    failures += purelibm_test_assert_true(purelibm_f32_is_zero(neg_zero_f32));
    failures += purelibm_test_assert_true(!purelibm_f32_sign_bit(pos_zero_f32));
    failures += purelibm_test_assert_true(purelibm_f32_sign_bit(neg_zero_f32));
    failures += purelibm_test_assert_true(purelibm_f32_is_inf(pos_inf_f32));
    failures += purelibm_test_assert_true(purelibm_f32_is_nan(nan_f32));
    failures +=
        purelibm_test_assert_true(purelibm_f32_is_subnormal(subnormal_f32));
    failures += purelibm_test_assert_u32(purelibm_f32_exponent_bits(pos_inf_f32),
                                         UINT32_C(0xff));
    failures += purelibm_test_assert_u32(purelibm_f32_mantissa_bits(nan_f32),
                                         UINT32_C(0x400001));

    failures += purelibm_test_assert_true(purelibm_f64_is_zero(pos_zero_f64));
    failures += purelibm_test_assert_true(purelibm_f64_is_zero(neg_zero_f64));
    failures += purelibm_test_assert_true(!purelibm_f64_sign_bit(pos_zero_f64));
    failures += purelibm_test_assert_true(purelibm_f64_sign_bit(neg_zero_f64));
    failures += purelibm_test_assert_true(purelibm_f64_is_inf(neg_inf_f64));
    failures += purelibm_test_assert_true(purelibm_f64_is_nan(nan_f64));
    failures +=
        purelibm_test_assert_true(purelibm_f64_is_subnormal(subnormal_f64));
    failures += purelibm_test_assert_u64(purelibm_f64_exponent_bits(neg_inf_f64),
                                         UINT64_C(0x7ff));
    failures += purelibm_test_assert_u64(purelibm_f64_mantissa_bits(nan_f64),
                                         UINT64_C(0x8000000000001));

    return failures == 0 ? 0 : 1;
}
