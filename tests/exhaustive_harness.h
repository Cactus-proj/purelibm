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

#endif
