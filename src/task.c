#include <linux/time.h>
#include <unistd.h>

#include "task.h"

/* Execution functions for tasks. */
static task_result_t ping(task_ctx_t *ctx);

/*
 * Execute a generic task using the information within the task.
 * Arguments:
 *     task_t *ctx - The task to execute.
 * Returns:
 *     task_reult_t - The result of executing the task.
 */
task_result_t task_execute(task_t *task) {
    task_result_t result = task->execute(task->ctx);
    return result;
}

/*
 * Generate a task from a task description (deserialization of a task).
 * Arguments:
 *     task_t *task - Write the task to this memory.
 *     task_desc_t *desc - The description of the task to generate.
 * Returns:
 *     int - 0 on success, -1 on failure.
 */
int task_generate(task_t *task, task_desc_t *desc) {
    switch(task->type) {
    case PING_TASK:
        
        break;
    default:
        return -1;
    }
}

/*
 * Execution function for a ping task.
 * Arguments:
 *     task_ctx_t *ctx - The task context.
 * Returns:
 *     task_result_t - The result of the ping.
 */
task_result_t ping(task_ctx_t *ctx) {
    struct timespec start, end;
    long seconds, nanoseconds;
    float elapsed;

    // Get the start of the execution
    clock_gettime(CLOCK_MONOTONIC, &start);

    char *node_id = ctx->node_state->node_id;
    uint16_t cpu_cores = ctx->node_state->capabilities.cpu_cores;
    float battery = ctx->node_state->live.battery_level;

    // Local log
    printf(
        "[PingTask] node=%s, cpus=%d, battery=%.2f",
        node_id, cpu_cores, battery
    );

    // Record metrics
    seconds = end.tv_sec - start.tv_sec;
    nanoseconds = end.tv_nsec - start.tv_nsec;

    if(nanoseconds < 0) {
        seconds--;
        nanoseconds += 1000000000;
    }

    elapsed = seconds + nanoseconds / 1e9;

    ctx->metrics->exec_time = elapsed;
    ctx->metrics->success = 1;
    
    return SUCCESS;
}