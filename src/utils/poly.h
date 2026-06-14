// SPDX-License-Identifier: MIT OR Apache-2.0
#ifndef PURELIBM_UTILS_POLY_H
#define PURELIBM_UTILS_POLY_H

#include <math.h>
#include <stddef.h>

static inline float purelibm_horner_f32(float x, const float *coeffs, size_t len) {
    if (len == 0) {
        return 0.0f;
    }

    float acc = coeffs[len - 1];
    for (size_t i = len - 1; i > 0; --i) {
        acc = fmaf(acc, x, coeffs[i - 1]);
    }
    return acc;
}

static inline double purelibm_horner_f64(double x, const double *coeffs, size_t len) {
    if (len == 0) {
        return 0.0;
    }

    double acc = coeffs[len - 1];
    for (size_t i = len - 1; i > 0; --i) {
        acc = fma(acc, x, coeffs[i - 1]);
    }
    return acc;
}

#endif
