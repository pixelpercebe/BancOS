#include <clock.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>

#include <os.h>
#include <sync_resources.h>
#include <OS_task/task_map.h>
#include <errors.h>
#include <utils.h>
#include <types.h>

static int seconds, nanoseconds;
u_llong microseconds;
pthread_t clock_tid;


static void print_info(int global_tick)
{
    printf("\n [CLOCK] tick: %d\n", global_tick);
}

/**
 * @brief Convierte la frecuencia de la CPU de GHz a segundos y nanosegundos.
 * @param freq_ghz Frecuencia de la CPU en GHz.
 * @param seconds Puntero para almacenar los segundos.
 * @param nanoseconds Puntero para almacenar los nanosegundos.
 */
void freq_to_seconds(float freq_ghz, int *seconds, int *nanoseconds) {
    float total_seconds = 1.0f / freq_ghz; // Convertir GHz a segundos
    *seconds = (int)total_seconds; // Parte entera
    *nanoseconds = (int)((total_seconds - *seconds) * 1e9); // Parte fraccionaria a nanosegundos
}

/**
 * @brief Función del hilo de reloj. Incrementa global_ticks y maneja la sincronización con los temporizadores.
 */
static void* clock_thread() {
    while(1){
        pthread_mutex_lock(&mutex);
        while (done < bancos.number_of_tasks) {
                pthread_cond_wait(&cond, &mutex);
        }
        done = 0;
        pthread_cond_broadcast(&cond2);
        global_ticks++;
        print_info(global_ticks);
        pthread_mutex_unlock(&mutex);
        usleep(microseconds);       
    }
    return NULL;
}


/**
 * @brief Inicializa el módulo de reloj. Crea el hilo de reloj y los hilos temporizadores.
 * Devuelve el ID del hilo de reloj o un código de error.
 * @param freq_cpu Frecuencia de la CPU en GHz.
 * @return ID del hilo de reloj o código de error.
 */
int init_clock_module(float freq_cpu) {
    // Declaracion y Asignacion de Memoria Dinamica
    // array de IDs de temporizadores
    global_ticks = 0;
    done = 0;   

    // Creación del Hilo Clock
    if (pthread_create(&clock_tid, NULL, clock_thread, NULL) != 0) {
        perror("Error al crear el hilo clock.");
        return ERR_CLOCK_INIT;
    }
    
    freq_to_seconds(freq_cpu, &seconds, &nanoseconds);
    calculate_microseconds(seconds, nanoseconds, &microseconds);
    printf("Frecuencia CPU: %.2f GHz -> Tiempo de tick: %d segundos y %d nanosegundos\n", freq_cpu, seconds, nanoseconds);
    return clock_tid;
}



/**
 * @brief Obtiene el valor actual de global_ticks.
 * @return Valor de global_ticks.
 */
int get_current_global_ticks() {
    return global_ticks;
}