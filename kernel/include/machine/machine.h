#ifndef MACHINE_H
#define MACHINE_H
#include <types.h>

typedef struct{
    u_int num_cpu;
    u_int cpu_num_cores;
    float cpu_clock_speed_Ghz;
    u_int cpu_hardware_threads;
    u_int cpu_cache_L1;
    u_int cpu_cache_L2;
    u_int cpu_cache_L3;
    char * replacement_policy;
    u_int scheduler_tick_freq;
    u_int total_cores; //no se puede calcular en tiempo de compilacion
    u_int total_hardware_threads; //no se puede calcular en tiempo de compilacion
} Machine;


#define FIELD_NUM_CPU "num_cpu"
#define FIELD_CORES "cpu_num_cores"
#define FIELD_CLOCK_SPEED "cpu_clock_speed_Ghz"
#define FIELD_THREADS "cpu_hardware_threads"
#define FIELD_CACHE_L1 "cpu_cache_L1"
#define FIELD_CACHE_L2 "cpu_cache_L2"
#define FIELD_CACHE_L3 "cpu_cache_L3"
#define FIELD_REPLACEMENT_POLICY "replacement_policy"
#define FIELD_SCHED_TICK_FREQ "scheduler_tick_freq"

#endif // MACHINE_H