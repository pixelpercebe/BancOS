#ifndef MACHINE_H
#define MACHINE_H
#include <types.h>
#include <errors.h>

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

ErrorCode init_machine_architecture();
int load_machine_config(char * filepath);
ErrorCode init_cores_struct();

#endif // MACHINE_H