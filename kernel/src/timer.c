#include <timer.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>

#include <sync_resources.h>
#include <os.h>

pthread_t timer_tid;

/*
*********************************************************************************************************
*                                  Utilidades timers
*********************************************************************************************************
*/

/**
 * @brief Traza el estado del temporizador (solo en modo verbose).
 * @param timer_id Identificador del temporizador.
 * @param tick_count Contador de ticks actual del temporizador.
 * @param is_counting 1 si está ejecutando, 0 si imprime el tick alcanzado.
 */
static void print_info(int timer_id, int tick_count, int is_counting)
{
    if (!is_counting)
        VERBOSE_PRINTF("\n [TIMER %d] tick: %d\n", timer_id, tick_count);
    else
        VERBOSE_PRINTF("\n [TIMER %d] ejecutando\n", timer_id);
}

/**
* @brief Genera un ID único para cada temporizador.
 * @return: ID único.
 */
static int get_random_id() {
    static int last_id = 0;
    return ++last_id;
}


/*
*********************************************************************************************************
*                                   Funciones del hilo temporizador.                                     
*
* Description: Esta función implementa la lógica del hilo temporizador. Incrementa el contador de ticks
*              y verifica si se ha alcanzado la duración especificada. Si es así, ejecuta la función de callback
*              asociada al temporizador.
*********************************************************************************************************
*/

/**
* @brief Función del hilo temporizador.
* @param arg: struct Timer
* @return: NULL
*/
static void* timer(void *arg) {
// Convertir el argumento void* a int* y obtener el ID
    Timer *timer = arg;
    int timer_id = timer->timer_id;
    int tick_count = 0;
    
    pthread_mutex_lock(&mutex);

    while (1) {
        print_info(timer_id, tick_count, 1);
        if (tick_count == timer->duration) {
            print_info(timer_id, tick_count, 0);
            if (timer->callback != NULL) {
                timer->callback();
            }
            tick_count = 0; // Reiniciar el contador de ticks
        }
        tick_count++;
        done++;
        pthread_cond_signal(&cond);
        pthread_cond_wait(&cond2, &mutex);
    }
    return NULL;
}


/**
* @brief Inicializa el módulo de temporizadores. Crea los hilos temporizadores. Devuelve OK o un código de error.
* @return: OK o código de error.
*/
ErrorCode init_timer_module(int duration, int is_active, void (*callback)(void), Timer * timer_struct) {

        timer_struct ->timer_id = get_random_id(); 
        timer_struct ->duration = duration; 
        timer_struct ->is_active = is_active;
        timer_struct ->callback = callback;

    if (pthread_create(&timer_tid, NULL, timer, timer_struct) != 0) {
        perror("Error al crear un hilo temporizador.");
        return ERR_TIMER_INIT;
    }
    return OK;
}

