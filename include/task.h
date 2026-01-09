#ifndef TASK_H
#define TASK_H

#include <stdint.h>

#include "chunk.h"
#include "node.h"

enum task_type {
    PING_TASK,
};

typedef enum {
    SUCCESS,
    FATAL_ERROR,
    RETRYABLE_ERROR,
    MISSING_CHUNK,
} task_result_t;

typedef struct {
    // --- Timing ---
    float exec_time;
    float cpu_time;

    // --- Resource Usage ---
    uint64_t peak_memory_bytes;
    uint64_t input_bytes;
    uint64_t output_bytes;

    // --- Mobile Specific ---
    float battery_before;   // 0.0 - 1.0
    float battery_after;
    int throttled;

    // --- Status ---
    int success;
} task_metrics_t;

typedef struct {
    chunk_store_t *chunks;
    node_state_t *node_state;
    task_metrics_t *metrics;
} task_ctx_t;

/* Represents a task for the worker nodes to complete. */
typedef struct {
    enum task_type  type;                           // Task type
    task_ctx_t      *ctx;                           // Context for the task to work with
    task_result_t   (*execute)(task_ctx_t *ctx);    // Execute function of the task
} task_t;

/* This is the serialized info that will be passed along to each worker node, which will then create a task. */
typedef struct {
    enum task_type  type;
    uint16_t        num_chunks;
    char            *chunks;
    char            *params;
} task_desc_t;

/*
 * Execute a generic task using the information within the task.
 * Arguments:
 *     task_t *task - The task to execute.
 * Returns:
 *     task_reult_t - The result of executing the task.
 */
task_result_t task_execute(task_t *task);

/*
 * Generate a task from a task description (deserialization of a task).
 * Arguments:
 *     task_t *task - Write the task to this memory.
 *     task_desc_t *desc - The description of the task to generate.
 * Returns:
 *     int - 0 on success, -1 on error.
 */
int task_generate(task_t *task, task_desc_t *desc);

#endif