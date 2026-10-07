/**
 * @file filter_service.h
 * @brief Industrial Digital Filtering Service (EMA & Median Filter)
 * @author Sherifred Singh
 * 
 * Provides integer-based fixed-point Exponential Moving Average (EMA) and
 * Median-3 filters for analog sensor noise suppression without floating-point overhead.
 */

#ifndef FILTER_SERVICE_H
#define FILTER_SERVICE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EMA_FILTER_SCALE_SHIFT  8U   /**< 256 scale factor (Q8 fixed-point) */
#define EMA_FILTER_SCALE        (1U << EMA_FILTER_SCALE_SHIFT)

/**
 * @brief Exponential Moving Average Filter State
 */
typedef struct {
    uint32_t state_q8;      /**< Accumulated state scaled by 256 */
    uint16_t alpha_q8;      /**< Smoothing factor (1 - 255). Higher = faster response, lower = heavier filtering */
    bool     is_initialized;
} ema_filter_t;

/**
 * @brief Median-3 Filter State (rejection of high-voltage impulse spikes)
 */
typedef struct {
    uint16_t samples[3];
    uint8_t  index;
    bool     is_full;
} median3_filter_t;

/**
 * @brief Initialize an Exponential Moving Average filter instance.
 * @param filter Pointer to filter struct
 * @param alpha_percent Alpha smoothing coefficient in percent (1% to 99%, e.g. 20% = 51 in Q8)
 */
void filter_ema_init(ema_filter_t *filter, uint8_t alpha_percent);

/**
 * @brief Reset the filter state with an initial reading.
 */
void filter_ema_reset(ema_filter_t *filter, uint16_t initial_val);

/**
 * @brief Update the EMA filter with a new raw sample.
 * @param filter Pointer to filter struct
 * @param new_sample New measurement
 * @return Filtered output value
 */
uint16_t filter_ema_update(ema_filter_t *filter, uint16_t new_sample);

/**
 * @brief Initialize a Median-3 filter.
 */
void filter_median3_init(median3_filter_t *filter);

/**
 * @brief Update the Median-3 filter and get the median output.
 */
uint16_t filter_median3_update(median3_filter_t *filter, uint16_t new_sample);

#ifdef __cplusplus
}
#endif

#endif /* FILTER_SERVICE_H */
