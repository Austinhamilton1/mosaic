#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

#include "chunk.h"
#include "sha256.h"

/*
 * Check if a chunk store owns data.
 * Arguments:
 *     chunk_store_t *store - The chunk store to check.
 *     char *chunk_id - 32 bit hash of the data.
 * Returns:
 *     int - 1 if the chunk store owns the data, 0 otherwise.
 */
int chunk_store_has(chunk_store_t *store, char *chunk_id) {
    // The data will be stored in the file {root}/sha256_{chunk_id}
    char final_path[MAX_PATH] = { 0 };
    snprintf(final_path, MAX_PATH-1, "%s/sha256_%s", store->root, chunk_id);

    // Check if the above file exists
    FILE *file;
    if((file = fopen(final_path, "r"))) {
        fclose(file);
        return 1;
    }
    return 0;
}

/*
 * Read data from the chunk store.
 * Arguments:
 *     chunk_store_t *store - Read from the chunk store.
 *     void *dest - Read data into this buffer.
 *     size_t n - Size of the expected data to be read.
 *     char *chunk_id - 32 bit hash of the data.
 * Returns:
 *     int - Number of bytes read on success, -1 on failure.
 */
int chunk_store_read(chunk_store_t *store, void *dest, size_t n, char *chunk_id) {
    // The data will be stored in the file {root}/sha256_{chunk_id}
    char final_path[MAX_PATH] = { 0 };
    snprintf(final_path, MAX_PATH-1, "%s/sha256_%s", store->root, chunk_id);

    // Read the data from the above file
    FILE *file;
    if((file = fopen(final_path, "rb"))) {
        size_t bytes_read = fread(dest, 1, n, file);
        fclose(file);

        return bytes_read;
    }
    return -1;
}

/*
 * Read count ints from the chunk store.
 * Arguments:
 *     chunk_store_t *store - Read from this chunk store.
 *     uint32_t *dest - Read data into this pointer.
 *     size_t count - Read this many ints into the pointer.
 *     char *chunk_id - 64 bit hash of the data.
 * Returns:
 *     int - Number of integers read on success, -1 on failure.
 */
int chunk_store_read_ints(chunk_store_t *store, uint32_t *dest, size_t count, char *chunk_id) {
    char buffer[sizeof(uint32_t) * count];
    int num_bytes = chunk_store_read(store, buffer, sizeof(buffer), chunk_id);

    if(num_bytes < 0) return -1;

    // Calculate the number of integers read from the chunk file
    int integers_read = num_bytes / sizeof(uint32_t);

    // Correct endianness issues
    char *ptr = buffer;
    for(int i = 0; i < integers_read; i++) {
        uint32_t non_normalized_int = ntohl((uint32_t)ptr);
        memcpy(ptr, &non_normalized_int, sizeof(uint32_t));
        ptr += sizeof(uint32_t);
    }

    // Copy the corrected bytes to the destination
    memcpy((char *)dest, buffer, integers_read);


    return integers_read;
}

/*
 * Write data to the chunk store.
 * Arguments:
 *     chunk_store_t *store - Write to this chunk store.
 *     void *src - Read data from this buffer.
 *     size_t n - Size of the data to write.
 * Returns:
 *     int - 0 on success, -1 on failure.
 */
int chunk_store_write(chunk_store_t *store, const void *src, size_t n) {
    // Calculate the chunk id for the data
    char chunk_id[SHA256_HEX_SIZE] = { 0 };
    sha256_hex(src, n, chunk_id);

    // The data will be stored in the file {root}/sha256_{chunk_id}
    char final_path[MAX_PATH] = { 0 };
    snprintf(final_path, MAX_PATH-1, "%s/sha256_%s", store->root, chunk_id);

    // Write the data to the above file
    FILE *file;
    if((file = fopen(final_path, "wb"))) {
        size_t bytes_written = fwrite(src, 1, n, file);
        fclose(file);

        if (bytes_written != n) return -1;

        return 0;
    }
    return -1;
}

/*
 * Write count ints to the chunk store.
 * Arguments:
 *     chunk_store_t *store - Write to this chunk store.
 *     const uint32_t *src - Read data from this pointer.
 *     size_t count - Number of ints to write.
 * Returns:
 *     int - 0 on success, -1 on failure.
 */
int chunk_store_write_ints(chunk_store_t *store, const uint32_t *src, size_t count) {
    // Ensure endianness is not an issue
    char buffer[sizeof(uint32_t) * count];
    char *ptr = buffer;
    for(int i = 0; i < count; i++) {
        uint32_t normalized_int = htonl(src[i]);
        memcpy(ptr, &normalized_int, sizeof(uint32_t));
        ptr += sizeof(uint32_t);
    }

    return chunk_store_write(store, buffer, sizeof(buffer));
}