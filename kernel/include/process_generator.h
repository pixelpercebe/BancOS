#ifndef PROCESS_GENERATOR_H
#define PROCESS_GENERATOR_H

#include <pcb.h>
#include <errors.h>

ErrorCode init_process_generator(int proc_gen_freq);
void generate_process();

#endif // PROCESS_GENERATOR_H