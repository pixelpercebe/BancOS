#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>


static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
static int seconds, nanoseconds;
static int num_temp;
static int done;
static uint64_t global_ticks;


static void* clock_thread() {
    while(1){
        pthread_mutex_lock(&mutex);
        while (done < num_temp) {
                pthread_cond_wait(&cond, &mutex);
        }
        done = 0;
        pthread_cond_broadcast(&cond2);
        global_ticks++;
        printf("reloj %llu\n", global_ticks);
        pthread_mutex_unlock(&mutex);
        usleep(seconds * 1000000 + nanoseconds / 1000); // 1 segundo
    }
}


static void* temporizador(void *arg) {
// 1. Convertir el argumento void* a int* y obtener el ID
    int *timer_id_ptr = arg;
    int timer_id = *timer_id_ptr;
    
    // 2. Liberar la memoria asignada para este ID
    
    pthread_mutex_lock(&mutex);
    while (1) {
        done ++;
        printf("temporizador %d tick %d\n", *timer_id_ptr, done);
        pthread_cond_signal(&cond);
        pthread_cond_wait(&cond2, &mutex);
    }
    free(timer_id_ptr);
}


static void freq_to_seconds(float freq_ghz, int *seconds, int *nanoseconds) {
    float total_seconds = 1.0f / freq_ghz; // Convertir GHz a segundos
    *seconds = (int)total_seconds; // Parte entera
    *nanoseconds = (int)((total_seconds - *seconds) * 1e9); // Parte fraccionaria a nanosegundos
}

int init_clock_module(float freq_cpu) {
    // Declaracion y Asignacion de Memoria Dinamica
    pthread_t clock_tid;
    // array de IDs de temporizadores
    global_ticks = 0;
    done = 0;   
    pthread_t *timer_tids = (pthread_t *)malloc(num_temp * sizeof(pthread_t));
    if (timer_tids == NULL) {
        perror("Error al asignar memoria para los hilos.");
        return 2;
    }

    // Creación del Hilo Clock
    if (pthread_create(&clock_tid, NULL, clock_thread, NULL) != 0) {
        perror("Error al crear el hilo clock.");
        free(timer_tids);
        return 3;
    }

    // 4. Creación de los Hilos Temporizadores en Bucle
    printf("Creando %d hilos temporizadores...\n", num_temp);
    for (int i = 0; i < num_temp; i++) {
        // Creamos un puntero a entero y le asignamos memoria para guardar el ID
        int *timer_id = (int *)malloc(sizeof(int));
        if (timer_id == NULL) {
            perror("Error al asignar memoria para el ID del temporizador");
            // Manejo de errores...
            break; 
        }
        *timer_id = i + 1;

        // En cada iteración, se crea un hilo y su ID se guarda en timer_tids[i]
        if (pthread_create(&timer_tids[i], NULL, temporizador, timer_id) != 0) {
            perror("Error al crear un hilo temporizador.");
            break; 
        }
    }

    freq_to_seconds(freq_cpu, &seconds, &nanoseconds);
    printf("Frecuencia CPU: %.2f GHz -> Tiempo de tick: %d segundos y %d nanosegundos\n", freq_cpu, seconds, nanoseconds);
    return clock_tid;
}

void wait_clock_module() {
    // pthread_join(clock_tid, NULL);
    // for (int i = 0; i < temp_kop; i++) {
    //     pthread_join(timer_tids[i], NULL);
    // }
    
    //while(1), esperar indefinidamente
    pause();
}

void set_num_timers(int n) {
    num_temp = n;
}

int get_global_ticks() {
    return global_ticks;
}