#include <scheduler.h>

#include <os.h>
#include <stdlib.h>
#include <errors.h>
#include <types.h>
#include <OS_task/task_def.h>
#include <OS_task/task_map.h>
#include <stdio.h>
#include <timer.h>
#include <pthread.h>
#include <pcb.h>
#include <machine/machine.h>
#include <machine/core.h>


const u_llong initial_budget = 100;

static void print_info()
{
    printf("\n [SCHED] ejecución del scheduler global\n");
}

/**
 * @brief Inicializa el módulo del scheduler global y locales.
 * @param tick_freq: Frecuencia de ticks del scheduler.
 * @return: Código de error.
 */
ErrorCode init_scheduler(int tick_freq)
{
    // Crear hilos de scheduler local para cada core
    for (int i = 0; i < bancos.machine_data.total_cores; i++) {
        Core *core = &bancos.cores[i];
        core->run_queue = (RunQueue *)malloc(sizeof(RunQueue));
        if (core->run_queue == NULL) return ERR_MEMORY_INSUFFICIENT;

        // Inicializamos la cola (función auxiliar o manual)
        core->run_queue = NULL; // Inicialización simple por ahora
        
        

        // Fijamos el precio base
        core->current_rent_price = PRECIO_ALQUILER_BASE;

        if (pthread_create(&core->thread_id, NULL, local_scheduler, (void *)core) != 0) {
            perror("Error al crear el hilo del core.");
            return  ERR_THREAD_CREATE;
        }
    }

    int ret = init_timer_module(tick_freq, TIMER_ACTIVE, scheduler, &bancos.task_map[SCHE]);
    if (ret == ERR_TIMER_INIT) exit(ERR_TIMER_INIT);
    print_task_map();

    return OK;
}



/**
 * @brief Función del scheduler.
 */
void scheduler()
{
    //u_llong budget = initial_budget;

    print_info();
    for (int i = 0; i < bancos.machine_data.total_cores; i++) {
        Core *core = &bancos.cores[i];

        // Despertar el core para que trabaje en este tick
        pthread_mutex_lock(&core->lock);
        core->should_work = 1;
        pthread_cond_signal(&core->wake_cond);
        pthread_mutex_unlock(&core->lock);
    }
}



// esta es la rutina de scheduler local de cada core, se llamara desde el task scheduler 
// que se ha guardado en el task map
// cada core tiene su propia runqueue y su propio scheduler local
// el scheduler local se encargara de seleccionar el siguiente proceso a ejecutar en ese core
// basandose en la politica de scheduling definida (CGS en este caso)
// y en el estado de la runqueue de ese core

/**
 * @brief Rutina del núcleo trabajador del scheduler.
 * @param arg: Argumento pasado a la rutina.
 * @return: Puntero nulo.
 */
void * local_scheduler(void *arg)
{
    Core *mi_core = (Core *)arg;
    while (1) {
        // 1. ESPERAR LA SEÑAL DEL RELOJ (DORMIR)
        pthread_mutex_lock(&mi_core->lock);
        while (mi_core->should_work == 0) {
            pthread_cond_wait(&mi_core->wake_cond, &mi_core->lock);
        }
        pthread_mutex_unlock(&mi_core->lock);

        // 2. TRABAJAR (PARALELO REAL)
        // Aquí es donde el core gestiona SU vivienda independientemente de los demás
        // a) Cobrar alquiler a los inquilinos (pisos)
        // b) Ejecutar instrucciones (simulado)
        // c) Si un inquilino se arruina -> Llamar Dispatcher (Desahucio)
        // d) Si hay sitio libre -> Llamar Scheduler Local (Buscar nuevo inquilino rico)
        
        printf("[Core %d] Procesando ciclo...\n", mi_core->core_id);
        //gestionar_vivienda(mi_core); 


        //completar con las funciones necesarias para gestionar la vivienda del core
        //llamada al dispatcher



        // 3. MARCAR TRABAJO TERMINADO
        pthread_mutex_lock(&mi_core->lock);
        mi_core->should_work = 0; // Me vuelvo a dormir hasta el siguiente aviso
        pthread_mutex_unlock(&mi_core->lock);
    }
    return NULL;
}


