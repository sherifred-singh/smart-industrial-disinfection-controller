/**
 * @file test_filter_service.c
 * @brief Unit Tests for EMA and Median Digital Filters
 * @author Sherifred Singh
 */

#include "unity.h"
#include "filter_service.h"

void test_ema_filter_step_response(void)
{
    ema_filter_t filter;
    filter_ema_init(&filter, 50);

    uint16_t out = filter_ema_update(&filter, 100);
    TEST_ASSERT_EQUAL_UINT(100, out);

    out = filter_ema_update(&filter, 200);
    TEST_ASSERT_EQUAL_UINT(150, out);

    out = filter_ema_update(&filter, 200);
    TEST_ASSERT_EQUAL_UINT(175, out);
}

void test_median3_filter_spike_rejection(void)
{
    median3_filter_t median;
    filter_median3_init(&median);

    filter_median3_update(&median, 50);
    filter_median3_update(&median, 52);
    uint16_t out = filter_median3_update(&median, 51);
    TEST_ASSERT_EQUAL_UINT(51, out);

    out = filter_median3_update(&median, 1000);
    TEST_ASSERT_EQUAL_UINT(52, out);
}

void run_test_filter_service(void)
{
    RUN_TEST(test_ema_filter_step_response);
    RUN_TEST(test_median3_filter_spike_rejection);
}
