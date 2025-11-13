#ifndef MACHINE_H
#define MACHINE_H
#include <types.h>

/*
num_cpu = 1
cpu_cores = 4
cpu_clock_speed_Ghz = 3.2
cpu_hardware_threads = 8
cpu_cache_L1 = 32KB
cpu_cache_L2 = 256KB
cpu_cache_L3 = 8MB
replacement_policy = Economical
*/

//global
typedef struct{
    const int num_cpu;
    const int cpu_num_cores;
    const double cpu_clock_speed_Ghz;
    const int cpu_hardware_threads;
    //static const char* cpu_cache_L1;
    //static const char* cpu_cache_L2;
    //static const char* cpu_cache_L3;
    //static const char* replacement_policy; 
} Machine;

#endif