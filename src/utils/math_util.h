#ifndef PURELIBM_UTILS_MATH_UTIL_H
#define PURELIBM_UTILS_MATH_UTIL_H

#include <math.h>

typedef struct {
    double hi;
    double lo;
} purelibm_dd;

static inline float purelibm_fma_f32(float a, float b, float c) {
    return fmaf(a, b, c);
}

static inline double purelibm_fma_f64(double a, double b, double c) {
    return fma(a, b, c);
}

static inline purelibm_dd purelibm_two_sum_f64(double a, double b) {
    double sum = a + b;
    double b_virtual = sum - a;
    double a_virtual = sum - b_virtual;
    double b_roundoff = b - b_virtual;
    double a_roundoff = a - a_virtual;
    purelibm_dd result = {sum, a_roundoff + b_roundoff};
    return result;
}

static inline purelibm_dd purelibm_quick_two_sum_f64(double a, double b) {
    double sum = a + b;
    double err = b - (sum - a);
    purelibm_dd result = {sum, err};
    return result;
}

static inline purelibm_dd purelibm_two_prod_f64(double a, double b) {
    double product = a * b;
    purelibm_dd result = {product, fma(a, b, -product)};
    return result;
}

#endif
