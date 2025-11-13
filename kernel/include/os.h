#pragma once

#include <types.h>
#include <errors.h>
#include <timer.h>
#include <pcb.h>
#include <process_generator.h>
#include <OS_task/task_map.h>
#include <machine/machine.h>

#define RUN_QUEUE_SIZE 10
#define TASK_LIST_LENGTH 4
#define MAX_PCB 10

typedef struct{
    Machine machine_data;

    PCB all_processes[MAX_PCB];
    int number_of_tasks;

    PCB ready_queue[RUN_QUEUE_SIZE];
    Timer task_map[TASK_LIST_LENGTH];
} system_t;

extern system_t bancos;