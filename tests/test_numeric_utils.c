#include "test_common.h"
#include "utils/ieee754.h"
#include "utils/math_util.h"
#include "utils/poly.h"

#include <stdint.h>

int main(void) {
    int failures = 0;

    const float coeffs_f32[] = {1.0f, 2.0f, 3.0f};
    const double coeffs_f64[] = {1.0, 2.0, 3.0};

    failures += purelibm_test_assert_f32_bits(
        purelibm_horner_f32(2.0f, coeffs_f32, 3),
        purelibm_f32_to_bits(17.0f));
    failures += purelibm_test_assert_f64_bits(
        purelibm_horner_f64(2.0, coeffs_f64, 3),
        purelibm_f64_to_bits(17.0));

    failures += purelibm_test_assert_f32_bits(
        purelibm_fma_f32(2.0f, 3.0f, 4.0f),
        purelibm_f32_to_bits(10.0f));
    failures += purelibm_test_assert_f64_bits(
        purelibm_fma_f64(2.0, 3.0, 4.0),
        purelibm_f64_to_bits(10.0));

    purelibm_dd sum = purelibm_two_sum_f64(1.0, 0x1p-53);
    failures +=
        purelibm_test_assert_f64_bits(sum.hi, purelibm_f64_to_bits(1.0));
    failures +=
        purelibm_test_assert_f64_bits(sum.lo, purelibm_f64_to_bits(0x1p-53));

    purelibm_dd quick_sum = purelibm_quick_two_sum_f64(1.0, 0x1p-53);
    failures += purelibm_test_assert_true(quick_sum.hi == 1.0);
    failures += purelibm_test_assert_true(quick_sum.lo == 0x1p-53);

    purelibm_dd product = purelibm_two_prod_f64(1.5, 2.0);
    failures +=
        purelibm_test_assert_f64_bits(product.hi, purelibm_f64_to_bits(3.0));
    failures +=
        purelibm_test_assert_f64_bits(product.lo, purelibm_f64_to_bits(0.0));

    return failures == 0 ? 0 : 1;
}
