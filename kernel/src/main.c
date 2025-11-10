// kernel/src/main.c
// Módulo principal del sistema operativo 
// gcc ./src/main.c ./src/clock.c ./src/timer.c ./src/utils.c ./src/process_generator.c src/sync_resources.c -o bancos -I include/ -pthread -lm
// mutex_cond 5 (para crear 5 temporizadores)

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>

#include <types.h>
#include <OS_task/task_map.h>
#include <clock.h>
#include <timer.h>
#include <errors.h>
#include <sync_resources.h>
/*
#define INT_ERROR(code, msg, value) \
    (fprintf(stderr, msg, value), code)
*/
float freq_cpu;
int n_timers;

static ErrorCode check_args(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <tiempo_en_ghz_CPU> <velocidad_generacion_procesos> <velocidad_dispatcher + scheduler> \n", argv[0]);
        return ERR_ARGS;
        //return INT_ERROR(1, "Uso: %s <numero_de_temporizadores> <tiempo_en_ghz>\n", argv[0]);
    }

    int n_timers = atoi(argv[1]);
    if (n_timers <= 0) {
        fprintf(stderr, "El número de temporizadores debe ser positivo.\n");
        return ERR_ARGS;
    }

    float freq_cpu = atof(argv[2]);
    if (freq_cpu <= 0) {
        fprintf(stderr, "La frecuencia de la CPU debe ser positiva.\n");
        return ERR_ARGS;
    }
    return OK;
}

int main(int argc, char *argv[]) {
    ErrorCode err = check_args(argc, argv);
    if (err != OK) {
        return err;
    }

    int clock_tid = init_clock_module(freq_cpu);
    if (clock_tid == ERR_CLOCK_INIT) {
        fprintf(stderr, "Error al inicializar el módulo de reloj. Código de error: %d\n", clock_tid);
        return clock_tid;
    
    }

    init_scheduler();
    
    //while(1), esperar indefinidamente
    pause(); 
    return 0;
}