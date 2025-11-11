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
#include <scheduler.h>
#include <process_generator.h>
#include <clock.h>
#include <timer.h>
#include <errors.h>
#include <sync_resources.h>
/*
#define INT_ERROR(code, msg, value) \
    (fprintf(stderr, msg, value), code)
*/

/**
 * @brief Verifica y procesa los argumentos de la línea de comandos.
 * @param argc: Número de argumentos.
 * @param argv: Array de argumentos.
 * @param freq_cpu: Puntero para almacenar la frecuencia de la CPU.
 * @param scheduler_tick_freq: Puntero para almacenar la frecuencia del scheduler.
 * @param proc_gen_freq: Puntero para almacenar la frecuencia del generador de procesos.
 * @return: Código de error.
 */
static ErrorCode check_args(int argc, char* argv[], float * freq_cpu,  int *scheduler_tick_freq, int *proc_gen_freq) {

    if (argc != 4) {
        fprintf(stderr, "Uso: %s <tiempo_en_ghz_CPU> <velocidad_dispatcher + scheduler> <velocidad_proceso_generador>\n", argv[0]);
        return ERR_ARGS;
        //return INT_ERROR(1, "Uso: %s <numero_de_temporizadores> <tiempo_en_ghz>\n", argv[0]);
    }


    *freq_cpu = (float) atof(argv[1]);
    if (*freq_cpu <= 0) {
        fprintf(stderr, "La frecuencia de la CPU debe ser positiva.\n");
        return ERR_ARGS;
    }

    *scheduler_tick_freq = atoi(argv[2]);
    if (*scheduler_tick_freq <= 0) {
        fprintf(stderr, "La frecuencia del scheduler debe ser positiva.\n");
        return ERR_ARGS;
    }

    *proc_gen_freq = atoi(argv[3]);
    if (*proc_gen_freq <= 0) {
        fprintf(stderr, "La frecuencia del generador de procesos debe ser positiva.\n");
        return ERR_ARGS;
    }


    return OK;
}

int main(int argc, char *argv[]) {
    float freq_cpu;
    int scheduler_tick_freq;
    int proc_gen_freq;
    ErrorCode err = check_args(argc, argv, &freq_cpu, &scheduler_tick_freq, &proc_gen_freq);
    if (err != OK) {
        return err;
    }

    int clock_tid = init_clock_module(freq_cpu);
    if (clock_tid == ERR_CLOCK_INIT) {
        fprintf(stderr, "Error al inicializar el módulo de reloj. Código de error: %d\n", clock_tid);
        return clock_tid;
    
    }

    init_scheduler(scheduler_tick_freq);
    init_process_generator(proc_gen_freq);

    //while(1), esperar indefinidamente
    pause(); 
    return 0;
}