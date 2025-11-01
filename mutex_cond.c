#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
int num_temp;
int done;

void* clock_thread() {
    while(1){
        
        pthread_mutex_lock(&mutex);
        while (done < num_temp) {
                pthread_cond_wait(&cond, &mutex);
        }
        done = 0;
        pthread_cond_broadcast(&cond2);
        printf("reloj\n");
        pthread_mutex_unlock(&mutex);
        usleep(1000000); // 0.5 segundos
    }
}


void * temporizador(void *arg) {
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

/*
void main(int argc, char *argv[]) {
    pthread_t thread1, thread2;
    num_temp = 2;
    done = 0;
        pthread_create(&thread1, NULL, clock, NULL);
        pthread_create(&thread2, NULL, temporizador, NULL);

        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);
    return;
}*/

int main(int argc, char *argv[]) {
    // gcc mutex.cond.c -pthread -o mutex_cond
    //mutex_cond 5 (para crear 5 temporizadores) 
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <numero_de_temporizadores>\n", argv[0]);
        return 1;
    }
    num_temp = atoi(argv[1]);
    if (num_temp <= 0) {
        fprintf(stderr, "El número de temporizadores debe ser positivo.\n");
        return 1;
    }

    // Declaracion y Asignacion de Memoria Dinamica
    pthread_t clock_tid;
    // array de IDs de temporizadores
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

    // pthread_join(clock_tid, NULL);
    // for (int i = 0; i < temp_kop; i++) {
    //     pthread_join(timer_tids[i], NULL);
    // }
    
    //while(1), esperar indefinidamente
    pause(); 

    // Liberación de recursos
    free(timer_tids);
    return 0;
}