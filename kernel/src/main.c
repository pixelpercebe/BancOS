// kernel/src/main.c
// Módulo principal del sistema operativo 
// gcc ./src/main.c ./src/clock.c ./src/timer.c ./src/utils.c ./src/process_generator.c src/sync_resources.c -o bancos -I include/ -pthread -lm
// mutex_cond 5 (para crear 5 temporizadores)

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#include <types.h>
#include <scheduler.h>
#include <process_generator.h>
#include <machine/machine.h>
#include <clock.h>
#include <timer.h>
#include <errors.h>
#include <sync_resources.h>
#include <os.h>
#include <sys_paths.h>
#include <utils.h>

/*
#define INT_ERROR(code, msg, value) \
    (fprintf(stderr, msg, value), code)
*/
//verbose mode variable
static int verbose_mode;
static int proc_gen_freq = 10; //frecuencia por defecto de generacion de procesos

/**
 * @brief Imprime la ayuda de uso del binario principal.
 * @param name Nombre del ejecutable.
 */
void print_help(char * name){
    fprintf(stderr, "\n----- BancOS Kernel -----\n");
    fprintf(stderr, "Uso: %s [-confile][-fcpu][-ncpu][-tcpu][-schedticks]\n\n", name);
    fprintf(stderr, "* No importa el orden de los parametros *\n\n");

    fprintf(stderr, "DESCRIPCIÓN:\n");
    fprintf(stderr, "  Simula el comportamiento de un kernel, gestionando ciclos de CPU, planificación\n");
    fprintf(stderr, "  de procesos y políticas de reemplazo.\n\n");

    fprintf(stderr, "OPCIONES DE CONFIGURACIÓN:\n");
    fprintf(stderr, "  -verbose              Activa el modo verbose para mostrar información detallada.\n");
    fprintf(stderr, "  -fgen                 frecuencia de la generación de procesos aleatorios.\n");
    fprintf(stderr, "  -confile <archivo>    Carga la configuración desde 'config/<archivo>'.\n");
    fprintf(stderr, "                        Por defecto: 'conf/config.ini'.\n");

    fprintf(stderr, "PARÁMETROS DE HARDWARE (Sobrescriben el archivo de configuración):\n");
    fprintf(stderr, "  -fcpu <float>         Frecuencia de la CPU en GHz.\n");
    fprintf(stderr, "  -ncpu <int>           Número de CPUs disponibles.\n");
    fprintf(stderr, "  -ncores <int>         Número de núcleos (cores) disponibles.\n");
    fprintf(stderr, "  -tcores <int>         Número de hilos (threads) por núcleo.\n\n");

    fprintf(stderr, "PARÁMETROS DE PLANIFICACIÓN:\n");
    fprintf(stderr, "  -schedticks <int>     Número de ticks de reloj entre ejecuciones del scheduler.\n\n");

    fprintf(stderr, "EJEMPLO:\n");
    fprintf(stderr, "  %s -confile custom.ini -fcpu 1 -ncpu 4 -ncores 6 -schedticks 5\n", name);
    fprintf(stderr, "----------------------------------------------------------------------\n");
}

/**
 * @brief Verifica y procesa los argumentos de la línea de comandos.
 * @param argc: Número de argumentos.
 * @param argv: Array de argumentos.
 * @return: Código de error.
 */
/**
 * @brief Procesa y carga los argumentos de línea de comandos.
 * @param argc Número de argumentos.
 * @param argv Vector de argumentos.
 * @return `OK` si se cargaron correctamente, `ERR_ARGS` si hay error de formato,
 *         `ABORT` si se solicitó ayuda.
 */
static ErrorCode load_args(int argc, char* argv[]) {
    if(argc == 1){
        load_machine_config(DEFAULT_FULLPATH_CONFIG);
        return OK;
    }
    if (argc == 2){
        if (strcmp(argv[1],PARAM_HELP)==0){
            print_help(argv[0]);
            return ABORT;
        }
        else{
            printf("\nERROR EN LOS ARGUMENTOS\n");
            return ERR_ARGS;}
    }


    //carga el config.ini
    if (argc > 2 && strcmp(argv[1],PARAM_CONF_FILE)==0) {
        //el siguiente parametro es el archivo de configuracion si no, el default
        load_machine_config(argv[2]);
    }
    else
    {
         load_machine_config(DEFAULT_FULLPATH_CONFIG);
    }

    for (int i = 1; i < argc; i++){
        //verbose mode
        if (strcmp(argv[i],PARAM_VERBOSE)==0){
            VERBOSE_PRINTF("\nVerbose mode activated\n");
            verbose_mode = 1;
        }
        //ncores
        if (strcmp(argv[i],PARAM_NCORES)==0){
            if (i+1 <= argc){
                int temp_ncores;
                if (!safe_atoi(argv[++i], &temp_ncores) || temp_ncores <= 0) {
                    fprintf(stderr, "Error: Valor inválido para -ncores: '%s'. Debe ser un entero positivo.\n", argv[i]);
                    return ERR_ARGS;
                }
                bancos.machine_data.cpu_num_cores = temp_ncores;
                printf("\nncores: %d\n", bancos.machine_data.cpu_num_cores);
            }
        }
        if(strcmp(argv[i],PARAM_FCPU)==0){
            if (i+1 <= argc){
                float temp_fcpu;
                if (!safe_atof(argv[++i], &temp_fcpu) || temp_fcpu <= 0) {
                    fprintf(stderr, "Error: Valor inválido para -fcpu: '%s'. Debe ser un número positivo.\n", argv[i]);
                    return ERR_ARGS;
                }
                bancos.machine_data.cpu_clock_speed_Ghz = temp_fcpu;
                printf("\nfcpu: %.2f\n", bancos.machine_data.cpu_clock_speed_Ghz);
            }
        }
        if(strcmp(argv[i],PARAM_NCPU)==0){
            if (i+1 <= argc){
                int temp_ncpu;
                if (!safe_atoi(argv[++i], &temp_ncpu) || temp_ncpu <= 0) {
                    fprintf(stderr, "Error: Valor inválido para -ncpu: '%s'. Debe ser un entero positivo.\n", argv[i]);
                    return ERR_ARGS;
                }
                bancos.machine_data.cpu_num_cores = temp_ncpu;
                printf("\nncpu: %d\n", bancos.machine_data.cpu_num_cores);
            }
        }
        if(strcmp(argv[i],PARAM_TCORES)==0){
            if (i+1 <= argc){
                int temp_tcores;
                if (!safe_atoi(argv[++i], &temp_tcores) || temp_tcores <= 0) {
                    fprintf(stderr, "Error: Valor inválido para -tcores: '%s'. Debe ser un entero positivo.\n", argv[i]);
                    return ERR_ARGS;
                }
                bancos.machine_data.cpu_hardware_threads = temp_tcores;
                printf("\ntcores: %d\n", bancos.machine_data.cpu_hardware_threads);
            }
        }
        if(strcmp(argv[i],PARAM_SCHEDTICKS)==0){
            if (i+1 <= argc){
                int temp_schedticks;
                if (!safe_atoi(argv[++i], &temp_schedticks) || temp_schedticks <= 0) {
                    fprintf(stderr, "Error: Valor inválido para -schedticks: '%s'. Debe ser un entero positivo.\n", argv[i]);
                    return ERR_ARGS;
                }
                bancos.machine_data.scheduler_tick_freq = temp_schedticks;
                printf("\nschedticks: %d\n", bancos.machine_data.scheduler_tick_freq);
            }
        }
        if(strcmp(argv[i],PARAM_GRANULARITY)==0){
            if (i+1 <= argc){
                int temp_granularity;
                if (!safe_atoi(argv[++i], &temp_granularity) || temp_granularity <= 0) {
                    fprintf(stderr, "Error: Valor inválido para -granularity: '%s'. Debe ser un entero positivo.\n", argv[i]);
                    return ERR_ARGS;
                }
                bancos.bucket_cgs_granularity = temp_granularity;
                printf("\ngranularity: %d\n", bancos.bucket_cgs_granularity);
            }
        }
        if(strcmp(argv[i],PARAM_MAX_BUDGET)==0){
            if (i+1 <= argc){
                int temp_max_budget;
                if (!safe_atoi(argv[++i], &temp_max_budget) || temp_max_budget <= 0) {
                    fprintf(stderr, "Error: Valor inválido para -max_budget: '%s'. Debe ser un entero positivo.\n", argv[i]);
                    return ERR_ARGS;
                }
                bancos.max_budget = temp_max_budget;
                printf("\nmax_budget: %d\n", bancos.max_budget);
            }
        }
        if(strcmp(argv[i],PARAM_FGEN)==0){
            if (i+1 <= argc){
                int temp_fgen;
                if (!safe_atoi(argv[++i], &temp_fgen) || temp_fgen <= 0) {
                    fprintf(stderr, "Error: Valor inválido para -fgen: '%s'. Debe ser un entero positivo.\n", argv[i]);
                    return ERR_ARGS;
                }
                proc_gen_freq = temp_fgen;
                printf("\nfgen: %d\n", temp_fgen);
            }
        }
    }
    
    return OK;
}


/**
 * @brief Punto de entrada del kernel de BancOS.
 *
 * Inicializa la arquitectura, el sistema, el generador de procesos y el reloj.
 * @param argc Número de argumentos.
 * @param argv Vector de argumentos.
 * @return Código de error o 0 si finaliza correctamente.
 */
int main(int argc, char *argv[]) {
    verbose_mode = 0; //default verbose mode off
    ErrorCode err = load_args(argc, argv);
    if (err != OK) {
        return err;
    }
    if (err == ABORT){
        return OK;
    }

    if (init_machine_architecture() != OK){
        printf("Error al inicializar la arquitectura de la máquina\n");
        exit(ERR_MACHINE_ARCH_INIT);
    }
    system_init(); //inicializa el sistema (scheduler, colas, etc)
    bancos.verbose_mode = verbose_mode;
    init_process_generator(proc_gen_freq); //borrar en la parte 3, solo para pruebas

    int clock_tid = init_clock_module(bancos.machine_data.cpu_clock_speed_Ghz);
    if (clock_tid == ERR_CLOCK_INIT) {
        fprintf(stderr, "Error al inicializar el módulo de reloj. Código de error: %d\n", clock_tid);
        return clock_tid;
    }

    //while(1), esperar indefinidamente
    pause(); 
    return 0;
}