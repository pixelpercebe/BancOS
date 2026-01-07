#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <types.h>
#include <pcb.h>
#include <errors.h>
#include <machine/core.h>

#define INFLATION_DIVIDER 10 //nunca debe ser 0
#define BITS_PER_WORD 32

// SALARIOS INICIALES: Con cuánto dinero nace cada clase (Define su bucket inicial)
// y cuanto ganan al volver al scheduler
static const int INITIAL_SALARY[] = {
    50,    // VAGABUNDO: Bucket 5
    200,   // BAJA:      Bucket 20
    800,   // MEDIA:     Bucket 80
    5000,  // ALTA:      Bucket 500
    20000  // ELITE:     Bucket 2000
};

// IMPUESTOS (ALQUILER): Cuánto pagan por cada tick de CPU
//    - los ricos pagan menos proporcionalmente.
static const int TAX_PER_TICK[] = {
    5,     // VAGABUNDO: Pierde dinero despacio
    10,    // BAJA
    25,    // MEDIA: La clase media paga mucho ;)
    100,   // ALTA: Gastan rápido
    200    // ELITE: Gastan mucho, pero tienen muchísimo más
};

//TIME SLICE (Quantum): Cuántos ticks seguidos pueden usar la CPU
//Los ricos tienen derecho a vivir más tiempo sin ser molestados.
static const int QUANTUM_BASE[] = {
    8,     // VAGABUNDO: Muy poco tiempo
    20,    // BAJA
    40,    // MEDIA
    80,    // ALTA
    150    // ELITE: Barra libre
};

typedef struct Bucket {
    PCB *head;
    PCB *tail;
} Bucket;

// Estructuras del Scheduler
typedef struct RunQueue {
    Bucket *buckets; //array dinámico de punteros a PCB

    u_int * active_bitmap; // bitmap de buckets activos
    u_long bitmap_size; // Tamaño del array de ints del bitmap

    u_int count;     // número de procesos en la cola
    u_int num_buckets; // número de buckets
    u_int max_active_bucket; // bucket más alto que tiene procesos
} RunQueue;

extern PCB waiting_proc_vec[];

ErrorCode init_scheduler(int tick_freq);
void scheduler();
void * local_core_scheduler(void *arg);
ErrorCode dispatcher(Core *core);
ErrorCode get_valid_core(Core **out_core);
ErrorCode scheduler_admit_process(Core *core, PCB *process);
int get_core_load(Core *c);

#endif // SCHEDULER_H

/*
el sched tiene un presupuesto default,
cada proceso tiene un presupuesto para alquilar la cpu
cada vez que un nuevo proceso entra durante su ejecucion su dinero disminuye

*/