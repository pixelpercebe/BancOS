#include <scheduler.h>
#include <errors.h>
#include <types.h>
#include <OS_task/task_def.h>
#include <OS_task/task_map.h>
#include <stdio.h>
#include <timer.h>
//&list[index]
//list + index

ErrorCode init_scheduler()
{
    // Initialize the task scheduler
    // Set up task lists, priority levels, etc.
    init_timer_module(10, TIMER_ACTIVE, scheduler, &task_map[SCHE]);
    print_task_map();
    return OK;
}



void scheduler()
{
    printf("IAM SCHEDULER");
}


