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

/**
 * @brief Establece el número de temporizadores que el reloj debe sincronizar.
 * @param n Número de temporizadores.
 */
static void set_num_timers(int n) {
    num_timers = n;
}

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

//(array de timers) for i in array -> create thread
/*
* Inicializa el módulo de temporizadores. Crea los hilos temporizadores.
* Devuelve OK o un código de error.
* num_temp: Número de temporizadores a crear.
* return: OK o código de error.
*/
ErrorCode init_timer_module() {

    pthread_t *timer_tids = (pthread_t *)malloc(num_timers * sizeof(pthread_t));
    if (timer_tids == NULL) {
        perror("Error al asignar memoria para los hilos.");
        return ERR_TIMER_INIT;
    }
    /*
    // 4. Creación de los Hilos Temporizadores en Bucle
    printf("Creando %d hilos temporizadores...\n", num_timers);
    for (int i = 0; i < num_timers; i++) {
        // Creamos un puntero a entero y le asignamos memoria para guardar el ID
        int *timer_id = (int *)malloc(sizeof(int));
        if (timer_id == NULL) {
            perror("Error al asignar memoria para el ID del temporizador");

            free(timer_tids);
            return ERR_MEMORY_INSUFFICIENT;
        }
        *timer_id = i + 1;
    */
   //TODO calcular id aleatorio para cada timer

    int timer_tid;
        // se crea un hilo y su ID se guarda en timer_tids[i]
    if (pthread_create(&timer_tid, NULL, timer, timer_id) != 0) {
        perror("Error al crear un hilo temporizador.");
        free(timer_id);
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
