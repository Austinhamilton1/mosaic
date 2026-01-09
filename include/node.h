#ifndef NODE_H
#define NODE_H

#include <stdint.h>
#include <time.h>

#include "sha256.h"

enum cpu_arch {
    ARM_32,
    ARM_64,
    X86,
    X64,
};

/* Basic hardware capabilities of a node. */
struct node_capabilities {
    uint16_t cpu_cores;     // Number of CPU cores
    uint64_t memory_bytes;  // Total available memory
    enum cpu_arch arch;     // Computer architecture
    int has_gpu;            // Has GPU capabilities
};

/* Live node statistics that change quickly over time. */
struct node_live_state {
    float cpu_load;         // Current CPU load (0.0 - 1.0)
    uint64_t mem_available; // Currently available memory
    float battery_level;    // Battery level (0.0 - 1.0 for battery devices < 0.0 for non-battery)
    float temperature_c;    // Temperature in Celcius (< 0.0 indicates no temperature available)
    int throttled;          // Is this device currently throttling?
};

/* Long term node statistics that change slowly over time. */
struct node_performance_profile {
    float avg_task_time;            // Approximate time to complete tasks overall
    float avg_cpu_time;             // Average amount of time spent on CPU
    float failure_rate;             // Average number of failures
    float battery_drain_per_sec;    // Approximate battery drain over time
    struct timespec last_seen;      // Last time the device checked in with scheduler
};

typedef struct {
    char node_id[SHA256_HEX_SIZE];

    // Static
    struct node_capabilities capabilities;

    // Dynamic (heartbeat)
    struct node_live_state live;

    // Learned
    struct node_performance_profile performance;
} node_state_t;

#endif