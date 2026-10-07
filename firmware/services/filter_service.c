/**
 * @file filter_service.c
 * @brief Digital Filter Service Implementation (EMA & Median-3)
 * @author Sherifred Singh
 */

#include "filter_service.h"
#include <string.h>

void filter_ema_init(ema_filter_t *filter, uint8_t alpha_percent)
{
    if (filter == NULL) {
        return;
    }

    if (alpha_percent == 0) {
        alpha_percent = 1;
    } else if (alpha_percent > 100) {
        alpha_percent = 100;
    }

    /* Convert percentage (1-100) to Q8 fixed point (1-256) */
    filter->alpha_q8       = (uint16_t)(((uint32_t)alpha_percent * EMA_FILTER_SCALE) / 100U);
    filter->state_q8       = 0;
    filter->is_initialized = false;
}

void filter_ema_reset(ema_filter_t *filter, uint16_t initial_val)
{
    if (filter != NULL) {
        filter->state_q8       = (uint32_t)initial_val << EMA_FILTER_SCALE_SHIFT;
        filter->is_initialized = true;
    }
}

uint16_t filter_ema_update(ema_filter_t *filter, uint16_t new_sample)
{
    if (filter == NULL) {
        return new_sample;
    }

    if (!filter->is_initialized) {
        filter_ema_reset(filter, new_sample);
        return new_sample;
    }

    /* EMA Formula: y[n] = alpha * x[n] + (1 - alpha) * y[n-1] */
    uint32_t sample_q8 = (uint32_t)new_sample << EMA_FILTER_SCALE_SHIFT;
    uint32_t alpha     = (uint32_t)filter->alpha_q8;
    uint32_t one_minus = EMA_FILTER_SCALE - alpha;

    filter->state_q8 = ((alpha * sample_q8) + (one_minus * filter->state_q8)) >> EMA_FILTER_SCALE_SHIFT;

    /* Round to nearest integer */
    return (uint16_t)((filter->state_q8 + (EMA_FILTER_SCALE / 2U)) >> EMA_FILTER_SCALE_SHIFT);
}

void filter_median3_init(median3_filter_t *filter)
{
    if (filter != NULL) {
        memset(filter, 0, sizeof(median3_filter_t));
    }
}

uint16_t filter_median3_update(median3_filter_t *filter, uint16_t new_sample)
{
    if (filter == NULL) {
        return new_sample;
    }

    filter->samples[filter->index] = new_sample;
    filter->index = (uint8_t)((filter->index + 1) % 3U);
    if (filter->index == 0) {
        filter->is_full = true;
    }

    if (!filter->is_full) {
        return new_sample;
    }

    uint16_t a = filter->samples[0];
    uint16_t b = filter->samples[1];
    uint16_t c = filter->samples[2];

    /* Fast Median of 3 elements */
    if ((a <= b && b <= c) || (c <= b && b <= a)) {
        return b;
    }
    if ((b <= a && a <= c) || (c <= a && a <= b)) {
        return a;
    }
    return c;
}
