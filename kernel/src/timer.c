#include <timer.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>

#include <sync_resources.h>

/*
* Función del hilo temporizador.
* arg: Puntero al ID del temporizador (entero).
* return: NULL.
*/

//parametros de timer = struct Timer
static void* timer(void *arg) {
// 1. Convertir el argumento void* a int* y obtener el ID
    Timer *timer = (Timer *)arg;
    int *timer_id_ptr = &timer->timer_id;
    int tick_count = 0;
    pthread_mutex_lock(&mutex);
    while (1) {
        tick_count++;
        printf("temporizador %d tick %d\n", *timer_id_ptr, tick_count);
        if (tick_count == timer->duration) {
            printf("Temporizador %d ha alcanzado su duración.\n", *timer_id_ptr);
        }
        done++;
        pthread_cond_signal(&cond);
        pthread_cond_wait(&cond2, &mutex);
    }
    free(timer_id_ptr);
    return NULL;
}


/**
* @brief Inicializa el módulo de temporizadores. Crea los hilos temporizadores. Devuelve OK o un código de error.
* @return: OK o código de error.
*/
ErrorCode init_timer_module(int duration, int is_active, void (*callback)(void)) {
    pthread_t timer_tid;
    pthread_t *timer_tids = (pthread_t *)malloc(sizeof(pthread_t));
    if (timer_tids == NULL) {
        perror("Error al asignar memoria para los hilos.");
        return ERR_TIMER_INIT;
    }

    #pragma region codigo de copilot para crear un struct Timer
        Timer *timer_arg = (Timer *)malloc(sizeof(Timer));
        if (timer_arg == NULL) {
            perror("Error al asignar memoria para el argumento del temporizador");
            free(timer_tids);
            return ERR_MEMORY_INSUFFICIENT;
        } 
        timer_arg->timer_id = 1; // Asignar un ID único aquí
        timer_arg->duration = 5; // Asignar una duración aquí
        timer_arg->is_active = 1; // Asignar el estado aquí
    #pragma endregion

    if (pthread_create(&timer_tid, NULL, timer, timer_arg) != 0) {
        perror("Error al crear un hilo temporizador.");
        free(timer_arg);
        return ERR_TIMER_INIT;
    }
    return timer_tid;
}





ErrorCode add_timer(int duration, int state) {
    // Implementación pendiente
    Timer *new_timer = (Timer *)malloc(sizeof(Timer));
    if (new_timer == NULL) {
        perror("Error al asignar memoria para el nuevo temporizador.");
        return ERR_MEMORY_INSUFFICIENT;
    }
    new_timer->duration = duration;
    new_timer->is_active = state;

    timers_list[last_id] = *new_timer;
    last_id++;
    init_timer_module();
    return OK;
}

ErrorCode remove_timer(int timer_id) {
    // Implementación pendiente
    return OK;
}

ErrorCode activate_timer(int timer_id) {
    // Implementación pendiente
    return OK;
}

ErrorCode deactivate_timer(int timer_id) {
    // Implementación pendiente
    return OK;
}
