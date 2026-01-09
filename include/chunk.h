#ifndef CHUNK_H
#define CHUNK_H

#include <stddef.h>

#define MAX_PATH 4096

typedef struct {
    char root[MAX_PATH-256];
} chunk_store_t;

/*
 * Check if a chunk store owns data.
 * Arguments:
 *     chunk_store_t *store - The chunk store to check.
 *     char *chunk_id - 64 bit hash of the data.
 * Returns:
 *     int - 1 if the chunk store owns the data, 0 otherwise.
 */
int chunk_store_has(chunk_store_t *store, char *chunk_id);

/*
 * Read data from the chunk store.
 * Arguments:
 *     chunk_store_t *store - Read from this chunk store.
 *     void *dest - Read data into this buffer.
 *     size_t n - Size of the expected data to be read.
 *     char *chunk_id - 64 bit hash of the data.
 * Returns:
 *     int - Number of btyes read on success, -1 on failure.
 */
int chunk_store_read(chunk_store_t *store, void *dest, size_t n, char *chunk_id);

/*
 * Read count ints from the chunk store.
 * Arguments:
 *     chunk_store_t *store - Read from this chunk store.
 *     int *dest - Read data into this pointer.
 *     size_t count - Read this many ints into the pointer.
 *     char *chunk_id - 64 bit hash of the data.
 * Returns:
 *     int - Number of integers read on success, -1 on failure.
 */
int chunk_store_read_ints(chunk_store_t *store, int *dest, size_t count, char *chunk_id);

/*
 * Write data to the chunk store.
 * Arguments:
 *     chunk_store_t *store - Write to this chunk store.
 *     const void *src - Read data from this buffer.
 *     size_t n - Size of the data to write.
 * Returns:
 *     int - 0 on success, -1 on failure.
 */
int chunk_store_write(chunk_store_t *store, const void *src, size_t n);

/*
 * Write count ints to the chunk store.
 * Arguments:
 *     chunk_store_t *store - Write to this chunk store.
 *     const int *src - Read data from this pointer.
 *     size_t count - Number of ints to write.
 * Returns:
 *     int - 0 on success, -1 on failure.
 */
int chunk_store_write_ints(chunk_store_t *store, const int *src, size_t count);

#endif