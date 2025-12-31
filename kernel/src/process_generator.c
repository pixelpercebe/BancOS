#include <process_generator.h>

#include <stdio.h>
#include <stdlib.h>

#include <os.h>
#include <OS_task/task_def.h>
#include <OS_task/task_map.h>
#include <errors.h>
#include <types.h>
#include <pcb.h>
#include <clock.h>


static void print_info()
{
    printf("\n [PROC_GEN] información del generador de procesos\n");
}


/**
 * @brief Imprime la lista de procesos generados.
 */
static void print_processes() {
    print_info();
    for (int i = 0; i < MAX_PCB; i++) {
        PCB p = bancos.all_processes[i];
        if (p.pid != 0) { // Asumiendo que un PID de 0 significa que el proceso no está inicializado
            printf("PID: %d, Lifetime: %d, Final Tick: %d, Budget: %d\n", p.pid, p.lifetime, p.final_tick, p.budget);
        }
    }
}

/*
*********************************************************************************************************
*                                 funciones del process generator.
*
* funciones base para el funcionamiento del generador de procesos.
*********************************************************************************************************
*/
/**
 * @brief Inicializa el módulo del generador de procesos.
 * @param proc_gen_freq: Frecuencia de generación de procesos en ticks.
 * @return: Código de error.
 */
ErrorCode init_process_generator(int proc_gen_freq)
{
    int ret = init_timer_module(proc_gen_freq,TIMER_ACTIVE, generate_process, &bancos.task_map[PROC_GEN]);
    if (ret == ERR_TIMER_INIT) exit(ERR_TIMER_INIT);
    print_task_map();
    return OK;
}


/**
 * @brief Genera un nuevo proceso y lo añade a la lista de procesos.
 */
void generate_process()
{
    static int process_count = 0;
    PCB new_process;

    new_process.pid = rand() % 1000; // Generar un PID aleatorio
    // Vida entre 50 y 250 ticks. Da tiempo a jugar con ellos.
    new_process.lifetime = (rand() % 200) + 50;
    new_process.final_tick = get_current_global_ticks() + new_process.lifetime;

    //clase social aleatoria
    int class_rand = rand() % 5;

    new_process.class = (SocialClass)class_rand;
    switch (new_process.class)
    {
    case VAGABUNDO:
        new_process.budget = SUELDO_INICIAL[VAGABUNDO];
        break;
    case BAJA:
        new_process.budget = SUELDO_INICIAL[BAJA];
        break;
    case MEDIA:
        new_process.budget = SUELDO_INICIAL[MEDIA];
        break;
    case ALTA:
        new_process.budget = SUELDO_INICIAL[ALTA];
        break;
    case ELITE:
        new_process.budget = SUELDO_INICIAL[ELITE];
        break;
    default:
        new_process.budget = SUELDO_INICIAL[MEDIA];
        break;
    } 

    bancos.all_processes[process_count] = new_process;
    process_count = (process_count + 1) % MAX_PCB;
    printf("Proceso generado: PID=%d, Lifetime=%d, Final Tick=%d ,salario=%d\n", new_process.pid, new_process.lifetime, new_process.final_tick, new_process.budget);
    print_processes();
}


