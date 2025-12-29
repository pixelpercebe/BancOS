#pragma once

#include <types.h>
#include <errors.h>
#include <timer.h>
#include <pcb.h>
#include <process_generator.h>
#include <machine/core.h>

#include <machine/machine.h> 

#define RUN_QUEUE_SIZE 10
#define TASK_LIST_LENGTH 4
#define MAX_PCB 10

#define PARAM_CONF_FILE "-confile"
#define PARAM_HELP "-help"
#define PARAM_NCPU "-ncpu"
#define PARAM_NCORES "-ncores"
#define PARAM_TCORES "-tcore"
#define PARAM_FCPU "-fcpu"
#define PARAM_RPOLICY "-rpolicy"
#define PARAM_SCHEDTICKS "-schedticks"


typedef struct{
    Machine machine_data;

    PCB all_processes[MAX_PCB];
    int number_of_tasks;
    Core *cores;

    PCB ready_queue[RUN_QUEUE_SIZE];
    Timer task_map[TASK_LIST_LENGTH];
} system_t;


extern system_t bancos;
int load_machine_config();
ErrorCode init_cores_struct();
void system_init();