// SPDX-License-Identifier: MIT OR Apache-2.0
#ifndef PURELIBM_TESTS_EXHAUSTIVE_HARNESS_H
#define PURELIBM_TESTS_EXHAUSTIVE_HARNESS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t start;
    uint32_t end;
    uint32_t current;
    bool started;
    bool done;
} purelibm_u32_range;

static inline purelibm_u32_range purelibm_u32_range_make(uint32_t start, uint32_t end) {
    purelibm_u32_range range = {start, end, start, false, start > end};
    return range;
}

static inline bool purelibm_u32_range_next(purelibm_u32_range *range, uint32_t *bits) {
    if (range->done) {
        return false;
    }

    if (!range->started) {
        range->started = true;
    } else if (range->current == range->end) {
        /* Last element already yielded — signal completion before incrementing
         * to avoid wrap-around when end == UINT32_MAX. */
        range->done = true;
        return false;
    } else {
        range->current += UINT32_C(1);
    }

    *bits = range->current;
    return true;
}

static inline uint64_t purelibm_count_u32_range(uint32_t start, uint32_t end) {
    if (start > end) {
        return UINT64_C(0);
    }
    return (uint64_t)end - (uint64_t)start + UINT64_C(1);
}

/* ================================================================
 * u64 range iterator — for f64 subrange scans and sampling.
 *
 * Full 2^64 sweeps are infeasible; these are used for boundary
 * Cartesian products, hard-to-round cases, and regression sets
 * as specified in DESIGN.md §4.4.
 * ================================================================ */

typedef struct {
    uint64_t start;
    uint64_t end;
    uint64_t current;
    bool started;
    bool done;
} purelibm_u64_range;

static inline purelibm_u64_range purelibm_u64_range_make(uint64_t start, uint64_t end) {
    purelibm_u64_range range = {start, end, start, false, start > end};
    return range;
}

static inline bool purelibm_u64_range_next(purelibm_u64_range *range, uint64_t *bits) {
    if (range->done) {
        return false;
    }

    if (!range->started) {
        range->started = true;
    } else if (range->current == range->end) {
        range->done = true;
        return false;
    } else {
        range->current += UINT64_C(1);
    }

    *bits = range->current;
    return true;
}

/* Returns 0 for empty ranges.  When the full [0, UINT64_MAX] range is
 * requested the count overflows uint64_t; we cap at UINT64_MAX. */
static inline uint64_t purelibm_count_u64_range(uint64_t start, uint64_t end) {
    if (start > end) {
        return UINT64_C(0);
    }
    /* Avoid overflow: end - start + 1 wraps when start=0, end=UINT64_MAX */
    uint64_t span = end - start;
    if (span == UINT64_MAX) {
        return UINT64_MAX;
    }
    return span + UINT64_C(1);
}

#endif
