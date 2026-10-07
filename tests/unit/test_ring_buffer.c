/**
 * @file test_ring_buffer.c
 * @brief Unit Tests for SPSC Ring Buffer
 * @author Sherifred Singh
 */

#include "unity.h"
#include "ring_buffer.h"

void test_ring_buffer_basic_push_pop(void)
{
    uint8_t storage[8];
    ring_buffer_t rb;
    ring_buffer_init(&rb, storage, 8);

    TEST_ASSERT_TRUE(ring_buffer_is_empty(&rb));
    TEST_ASSERT_FALSE(ring_buffer_is_full(&rb));
    TEST_ASSERT_EQUAL_UINT(0, ring_buffer_available(&rb));

    TEST_ASSERT_TRUE(ring_buffer_push(&rb, 0xAA));
    TEST_ASSERT_TRUE(ring_buffer_push(&rb, 0xBB));
    TEST_ASSERT_EQUAL_UINT(2, ring_buffer_available(&rb));

    uint8_t out = 0;
    TEST_ASSERT_TRUE(ring_buffer_pop(&rb, &out));
    TEST_ASSERT_EQUAL_UINT(0xAA, out);

    TEST_ASSERT_TRUE(ring_buffer_pop(&rb, &out));
    TEST_ASSERT_EQUAL_UINT(0xBB, out);

    TEST_ASSERT_TRUE(ring_buffer_is_empty(&rb));
}

void test_ring_buffer_overflow_protection(void)
{
    uint8_t storage[4];
    ring_buffer_t rb;
    ring_buffer_init(&rb, storage, 4);

    TEST_ASSERT_TRUE(ring_buffer_push(&rb, 1));
    TEST_ASSERT_TRUE(ring_buffer_push(&rb, 2));
    TEST_ASSERT_TRUE(ring_buffer_push(&rb, 3));
    TEST_ASSERT_TRUE(ring_buffer_is_full(&rb));

    TEST_ASSERT_FALSE(ring_buffer_push(&rb, 4));

    uint8_t val = 0;
    TEST_ASSERT_TRUE(ring_buffer_pop(&rb, &val));
    TEST_ASSERT_EQUAL_UINT(1, val);

    TEST_ASSERT_TRUE(ring_buffer_push(&rb, 4));
}

void run_test_ring_buffer(void)
{
    RUN_TEST(test_ring_buffer_basic_push_pop);
    RUN_TEST(test_ring_buffer_overflow_protection);
}
