#include <process_generator.h>

#include <stdio.h>
#include <stdlib.h>

#include <OS_task/task_def.h>
#include <OS_task/task_map.h>
#include <errors.h>
#include <types.h>
#include <pcb.h>
#include <clock.h>


PCB process[MAX_PCB];

static void print_processes() {
    printf("Lista de procesos generados:\n");
    for (int i = 0; i < MAX_PCB; i++) {
        PCB p = process[i];
        if (p.pid != 0) { // Asumiendo que un PID de 0 significa que el proceso no está inicializado
            printf("PID: %d, Lifetime: %d, Final Tick: %d\n", p.pid, p.lifetime, p.final_tick);
        }
    }
}

/*
*********************************************************************************************************
*                                     Process generator function
*
* Description: This function generates a new process with a unique PID,
*              a random lifetime, and calculates its final tick based on
*              the current global ticks.
*********************************************************************************************************
*/
ErrorCode init_process_generator(int proc_gen_freq)
{
    int ret = init_timer_module(proc_gen_freq,TIMER_ACTIVE, generate_process, &task_map[PROC_GEN]);
    if (ret == ERR_TIMER_INIT) exit(ERR_TIMER_INIT);
    print_task_map();
    return OK;
}

void generate_process()
{
    static int process_count = 0;
    PCB new_process;

    new_process.pid = rand() % 1000; // Generar un PID aleatorio
    new_process.lifetime = (rand() % 10) + 1; // Duración entre 1 y 10 ticks
    new_process.final_tick = get_current_global_ticks() + new_process.lifetime;


    process[process_count] = new_process;
    process_count = (process_count + 1) % MAX_PCB;
    printf("Proceso generado: PID=%d, Lifetime=%d, Final Tick=%d\n", new_process.pid, new_process.lifetime, new_process.final_tick);
    print_processes();
}


