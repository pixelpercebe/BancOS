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
#include <interruptions.h>

/**
 * @brief Imprime la lista de procesos generados.
 */
void print_all_processes() {
    pthread_mutex_lock(&bancos.list_pcb_lock);
    
    PCB *current = bancos.list_pcb_head;
    VERBOSE_PRINTF("\n--- LISTA GLOBAL DE PROCESOS (%d activos) ---\n", bancos.process_count);
    
    while (current != NULL) {
        VERBOSE_PRINTF("PID: %d | Budget: %d | Estado: %d\n", current->pid, current->budget, current->state);
        current = current->next_br;
    }
    
    pthread_mutex_unlock(&bancos.list_pcb_lock);
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

    PCB *new_process = (PCB *)calloc(1, sizeof(PCB));

    new_process->pid = rand() % 1000; // Generar un PID aleatorio
    // Vida entre 50 y 250 ticks. Da tiempo a jugar con ellos.
    new_process->lifetime = (rand() % 200) + 50;
    new_process->final_tick = get_current_global_ticks() + new_process->lifetime;
    //clase social aleatoria
    int class_rand = rand() % 5;
    new_process->class = (SocialClass)class_rand;

    new_process->state = NEW; // READY
    new_process->last_core = -1; // Inicializar con un valor inválido o predeterminado
    new_process->last_thread = -1; // Inicializar con un valor inválido o predeterminado
    new_process->budget = 0 ; // El scheduler le asignará presupuesto al ser admitido

    new_process->next_br = NULL;
    new_process->prev_br = NULL;
    insert_global_pcb(new_process);


    VERBOSE_PRINTF("Proceso generado: PID=%d, Lifetime=%d, Final Tick=%d ,salario=%d\n", new_process->pid, new_process->lifetime, new_process->final_tick, new_process->budget);
    new_process_interruption(new_process);

    //print_processes();
}


