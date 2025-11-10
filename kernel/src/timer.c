#include <timer.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>

#include <sync_resources.h>

pthread_t timer_tid;

/*
*********************************************************************************************************
*                                  Utilidades para la creación de timers.
*********************************************************************************************************
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
// 1. Convertir el argumento void* a int* y obtener el ID
    Timer *timer = arg;
    int timer_id = timer->timer_id;
    int tick_count = 0;
    
    pthread_mutex_lock(&mutex);

    while (1) {
        tick_count++;
        printf("temporizador %d tick %d\n", timer_id, tick_count);
        if (tick_count == timer->duration) {
            printf("Temporizador %d ha alcanzado su duración.\n", timer_id);
            if (timer->callback != NULL) {
                timer->callback();
            }
            tick_count = 0; // Reiniciar el contador de ticks
        }
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
ErrorCode init_timer_module(int duration, int is_active, void (*callback)(void), Timer * timer) {
    #pragma region Asignacion de Memoria Dinamica para Timer
        Timer *timer_arg = (Timer *)malloc(sizeof(Timer));
        if (timer_arg == NULL) {
            perror("Error al asignar memoria para el temporizador.");
            return ERR_MEMORY_INSUFFICIENT;
        }
        timer_arg->timer_id = get_random_id(); // Asignar un ID único aquí
        timer_arg->duration = duration; // Asignar una duración aquí
        timer_arg->is_active = is_active; // Asignar el estado aquí
        timer_arg->callback = callback; // Asignar la función de callback aquí
    
    #pragma endregion


    if (pthread_create(&timer_tid, NULL, timer, timer_arg) != 0) {
        perror("Error al crear un hilo temporizador.");
        free(timer_arg);
        return ERR_TIMER_INIT;
    }
    return OK;
}

