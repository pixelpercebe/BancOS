#pragma once

#include <types.h>
#include <errors.h>
#include <timer.h>
#include <pcb.h>
#include <process_generator.h>
#include <machine/core.h>
#include <scheduler.h>

#include <machine/machine.h> 

#define TASK_LIST_LENGTH 4

#define PARAM_CONF_FILE "-confile"
#define PARAM_HELP "-help"
#define PARAM_NCPU "-ncpu"
#define PARAM_NCORES "-ncores"
#define PARAM_TCORES "-tcore"
#define PARAM_FCPU "-fcpu"
#define PARAM_RPOLICY "-rpolicy"
#define PARAM_SCHEDTICKS "-schedticks"
#define PARAM_GRANULARITY "-granularity"
#define PARAM_MAX_BUDGET "-max_budget"
#define PARAM_VERBOSE "-verbose"

// Helper macro to print only when verbose_mode is enabled
#define VERBOSE_PRINTF(...) do { if (bancos.verbose_mode) printf(__VA_ARGS__); } while (0)


typedef struct system_t{
    Machine machine_data;

    // AÑADE ESTO: Estructura de Lista Global
    PCB *list_pcb_head; // Cabeza de la lista global
    PCB *list_pcb_tail; // Cola de la lista global
    int process_count;  // Contador actual
    pthread_mutex_t list_pcb_lock; // Mutex para proteger ESTA lista
    // -------------------



    u_int number_of_tasks; // número de tareas cargadas
    Core *cores; // Array dinámico de cores

    u_int bucket_cgs_granularity; // Granularidad de los buckets CGS
    u_int max_budget; // Presupuesto máximo para un proceso

    Timer task_map[TASK_LIST_LENGTH]; // Mapa de tareas del sistema

    int verbose_mode; // Modo verbose activado/desactivado
} system_t;


extern system_t bancos;
void system_init();
void insert_global_pcb(PCB *p);
void remove_global_pcb(PCB *p);