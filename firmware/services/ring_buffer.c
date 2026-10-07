/**
 * @file ring_buffer.c
 * @brief SPSC Lock-free Circular Buffer Implementation
 * @author Sherifred Singh
 */

#include "ring_buffer.h"

static inline bool is_power_of_two(size_t x)
{
    return (x != 0) && ((x & (x - 1)) == 0);
}

void ring_buffer_init(ring_buffer_t *rb, uint8_t *buffer, size_t capacity)
{
    if (rb == NULL || buffer == NULL || capacity == 0) {
        return;
    }

    rb->buffer   = buffer;
    rb->capacity = capacity;
    rb->head     = 0;
    rb->tail     = 0;

    if (is_power_of_two(capacity)) {
        rb->is_pow2 = true;
        rb->mask    = capacity - 1;
    } else {
        rb->is_pow2 = false;
        rb->mask    = 0;
    }
}

void ring_buffer_flush(ring_buffer_t *rb)
{
    if (rb != NULL) {
        rb->head = 0;
        rb->tail = 0;
    }
}

bool ring_buffer_push(ring_buffer_t *rb, uint8_t byte)
{
    if (rb == NULL || ring_buffer_is_full(rb)) {
        return false;
    }

    size_t next_head;
    if (rb->is_pow2) {
        rb->buffer[rb->head & rb->mask] = byte;
        next_head = (rb->head + 1) & rb->mask;
    } else {
        rb->buffer[rb->head] = byte;
        next_head = (rb->head + 1) % rb->capacity;
    }

    rb->head = next_head;
    return true;
}

bool ring_buffer_pop(ring_buffer_t *rb, uint8_t *byte)
{
    if (rb == NULL || byte == NULL || ring_buffer_is_empty(rb)) {
        return false;
    }

    if (rb->is_pow2) {
        *byte = rb->buffer[rb->tail & rb->mask];
        rb->tail = (rb->tail + 1) & rb->mask;
    } else {
        *byte = rb->buffer[rb->tail];
        rb->tail = (rb->tail + 1) % rb->capacity;
    }

    return true;
}

bool ring_buffer_peek(const ring_buffer_t *rb, uint8_t *byte)
{
    if (rb == NULL || byte == NULL || ring_buffer_is_empty(rb)) {
        return false;
    }

    if (rb->is_pow2) {
        *byte = rb->buffer[rb->tail & rb->mask];
    } else {
        *byte = rb->buffer[rb->tail];
    }

    return true;
}

size_t ring_buffer_write(ring_buffer_t *rb, const uint8_t *data, size_t len)
{
    if (rb == NULL || data == NULL || len == 0) {
        return 0;
    }

    size_t count = 0;
    while (count < len && ring_buffer_push(rb, data[count])) {
        count++;
    }
    return count;
}

size_t ring_buffer_read(ring_buffer_t *rb, uint8_t *data, size_t max_len)
{
    if (rb == NULL || data == NULL || max_len == 0) {
        return 0;
    }

    size_t count = 0;
    while (count < max_len && ring_buffer_pop(rb, &data[count])) {
        count++;
    }
    return count;
}

size_t ring_buffer_available(const ring_buffer_t *rb)
{
    if (rb == NULL) {
        return 0;
    }

    if (rb->head >= rb->tail) {
        return (rb->head - rb->tail);
    }
    return (rb->capacity - (rb->tail - rb->head));
}

size_t ring_buffer_free_space(const ring_buffer_t *rb)
{
    if (rb == NULL) {
        return 0;
    }
    return (rb->capacity - 1 - ring_buffer_available(rb));
}

bool ring_buffer_is_empty(const ring_buffer_t *rb)
{
    if (rb == NULL) {
        return true;
    }
    return (rb->head == rb->tail);
}

bool ring_buffer_is_full(const ring_buffer_t *rb)
{
    if (rb == NULL) {
        return false;
    }

    size_t next_head;
    if (rb->is_pow2) {
        next_head = (rb->head + 1) & rb->mask;
    } else {
        next_head = (rb->head + 1) % rb->capacity;
    }
    return (next_head == rb->tail);
}
