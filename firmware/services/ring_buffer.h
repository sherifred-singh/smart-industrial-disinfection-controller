/**
 * @file ring_buffer.h
 * @brief Lock-free Single-Producer Single-Consumer (SPSC) Circular Byte Buffer
 * @author Sherifred Singh
 */

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t *buffer;
    size_t   capacity;
    size_t   mask;       /**< Bitmask when capacity is a power of 2 */
    bool     is_pow2;
    volatile size_t head;
    volatile size_t tail;
} ring_buffer_t;

/**
 * @brief Initialize a ring buffer instance.
 * @param rb Pointer to ring buffer structure
 * @param buffer Underlying storage array
 * @param capacity Number of bytes in buffer
 */
void ring_buffer_init(ring_buffer_t *rb, uint8_t *buffer, size_t capacity);

/**
 * @brief Clear / flush all contents in the ring buffer.
 */
void ring_buffer_flush(ring_buffer_t *rb);

/**
 * @brief Push a single byte into the buffer.
 * @return true on success, false if buffer is full
 */
bool ring_buffer_push(ring_buffer_t *rb, uint8_t byte);

/**
 * @brief Pop a single byte from the buffer.
 * @return true on success, false if buffer is empty
 */
bool ring_buffer_pop(ring_buffer_t *rb, uint8_t *byte);

/**
 * @brief Peek at the next byte without removing it.
 */
bool ring_buffer_peek(const ring_buffer_t *rb, uint8_t *byte);

/**
 * @brief Write multiple bytes into the ring buffer.
 * @return Number of bytes actually written
 */
size_t ring_buffer_write(ring_buffer_t *rb, const uint8_t *data, size_t len);

/**
 * @brief Read multiple bytes from the ring buffer.
 * @return Number of bytes actually read
 */
size_t ring_buffer_read(ring_buffer_t *rb, uint8_t *data, size_t max_len);

/**
 * @brief Get the number of bytes currently stored in the buffer.
 */
size_t ring_buffer_available(const ring_buffer_t *rb);

/**
 * @brief Get the remaining free byte capacity in the buffer.
 */
size_t ring_buffer_free_space(const ring_buffer_t *rb);

/**
 * @brief Check if the buffer is empty.
 */
bool ring_buffer_is_empty(const ring_buffer_t *rb);

/**
 * @brief Check if the buffer is full.
 */
bool ring_buffer_is_full(const ring_buffer_t *rb);

#ifdef __cplusplus
}
#endif

#endif /* RING_BUFFER_H */
