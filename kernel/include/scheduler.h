#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <types.h>
#include <pcb.h>
#include <errors.h>
#include <machine/core.h>

extern PCB ready_process;

#define SUELDO_BASE 100
#define SALARIO_POBRE 100
#define SALARIO_MEDIA 500
#define SALARIO_ALTA  5000

#define PRECIO_ALQUILER_BASE 10


//SALARIOS INICIALES: Con cuánto dinero nace cada clase (Define su bucket inicial)
// y cuanto ganan al volver al scheduler
static const int SALARIO_INICIAL[] = {
    50,    // VAGABUNDO: Bucket 5
    200,   // BAJA:      Bucket 20
    800,   // MEDIA:     Bucket 80
    5000,  // ALTA:      Bucket 500
    20000  // ELITE:     Bucket 2000
};

// IMPUESTOS (ALQUILER): Cuánto pagan por cada tick de CPU
//    - los ricos pagan menos proporcionalmente.
static const int IMPUESTO_POR_TICK[] = {
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

// Estructuras del Scheduler
typedef struct RunQueue {
    PCB **buckets; //array dinámico de punteros a PCB
    PCB **tails;    //array dinamico de punteros a PCB (colas) de cada bucket
    u_int * active_bitmap; // bitmap de buckets activos

    u_int count;     // número de procesos en la cola
    u_int num_buckets; // número de buckets
    u_int max_active_bucket; // bucket más alto que tiene procesos
    u_int total_processes; // número total de procesos en la runqueue
} RunQueue;

extern PCB waiting_proc_vec[];

ErrorCode init_scheduler(int tick_freq);
void scheduler();
void * local_core_scheduler(void *arg);
ErrorCode dispatcher(Core *core);

#endif // SCHEDULER_H

/*
el sched tiene un presupuesto default,
cada proceso tiene un presupuesto para alquilar la cpu
cada vez que un nuevo proceso entra durante su ejecucion su dinero disminuye

*/