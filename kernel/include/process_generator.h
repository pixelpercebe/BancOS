#ifndef PROCESS_GENERATOR_H
#define PROCESS_GENERATOR_H

#include <pcb.h>
#include <errors.h>

#define MAX_PCB 10
extern PCB process[MAX_PCB];

ErrorCode init_process_generator(int proc_gen_freq);
void generate_process();

#endif // PROCESS_GENERATOR_H