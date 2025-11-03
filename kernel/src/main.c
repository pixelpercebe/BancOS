// kernel/src/main.c
// Módulo principal del sistema operativo 
// gcc mutex.cond.c -pthread -o mutex_cond
// mutex_cond 5 (para crear 5 temporizadores)

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>


/*
#define INT_ERROR(code, msg, value) \
    (fprintf(stderr, msg, value), code)
*/

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <numero_de_temporizadores> <tiempo_en_ghz>\n", argv[0]);
        return 1;
        //return INT_ERROR(1, "Uso: %s <numero_de_temporizadores> <tiempo_en_ghz>\n", argv[0]);
    }

    int num_temp = atoi(argv[1]);
    if (num_temp <= 0) {
        fprintf(stderr, "El número de temporizadores debe ser positivo.\n");
        return 1;
    }

    float freq_cpu = atof(argv[2]);
    if (freq_cpu <= 0) {
        fprintf(stderr, "La frecuencia de la CPU debe ser positiva.\n");
        return 1;
    }

    
    //while(1), esperar indefinidamente
    pause(); 
    return 0;
}