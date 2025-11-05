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
static void* timer(void *arg) {
// 1. Convertir el argumento void* a int* y obtener el ID
    int *timer_id_ptr = arg;

    pthread_mutex_lock(&mutex);
    while (1) {
        done ++;
        printf("temporizador %d tick %d\n", *timer_id_ptr, done);
        pthread_cond_signal(&cond);
        pthread_cond_wait(&cond2, &mutex);
    }
    free(timer_id_ptr);
    return NULL;
}



/*
* Inicializa el módulo de temporizadores. Crea los hilos temporizadores.
* Devuelve OK o un código de error.
* num_temp: Número de temporizadores a crear.
* return: OK o código de error.
*/
ErrorCode init_timer_module(int temp) {
    set_num_timers(temp);

    pthread_t *timer_tids = (pthread_t *)malloc(num_temp * sizeof(pthread_t));
    if (timer_tids == NULL) {
        perror("Error al asignar memoria para los hilos.");
        return ERR_TIMER_INIT;
    }

    // 4. Creación de los Hilos Temporizadores en Bucle
    printf("Creando %d hilos temporizadores...\n", num_temp);
    for (int i = 0; i < num_temp; i++) {
        // Creamos un puntero a entero y le asignamos memoria para guardar el ID
        int *timer_id = (int *)malloc(sizeof(int));
        if (timer_id == NULL) {
            perror("Error al asignar memoria para el ID del temporizador");

            free(timer_tids);
            return ERR_MEMORY_INSUFFICIENT;
        }
        *timer_id = i + 1;

        // En cada iteración, se crea un hilo y su ID se guarda en timer_tids[i]
        if (pthread_create(&timer_tids[i], NULL, timer, timer_id) != 0) {
            perror("Error al crear un hilo temporizador.");
            free(timer_id);
            return ERR_TIMER_INIT;
        }
    }
    return OK;
}

/**
 * @brief Establece el número de temporizadores que el reloj debe sincronizar.
 * @param n Número de temporizadores.
 */
const void set_num_timers(int n) {
    num_temp = n;
}