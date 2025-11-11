#include <scheduler.h>

#include <stdlib.h>
#include <errors.h>
#include <types.h>
#include <OS_task/task_def.h>
#include <OS_task/task_map.h>
#include <stdio.h>
#include <timer.h>


/**
 * @brief Inicializa el módulo del scheduler.
 * @param tick_freq: Frecuencia de ticks del scheduler.
 * @return: Código de error.
 */
ErrorCode init_scheduler(int tick_freq)
{
    int ret = init_timer_module(tick_freq, TIMER_ACTIVE, scheduler, &task_map[SCHE]);
    if (ret == ERR_TIMER_INIT) exit(ERR_TIMER_INIT);
    print_task_map();
    return OK;
}


/**
 * @brief Función del scheduler.
 */
void scheduler()
{
    printf("\nIAM SCHEDULER\n");
}


