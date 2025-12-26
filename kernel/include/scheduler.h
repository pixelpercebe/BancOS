#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <types.h>
#include <pcb.h>
#include <errors.h>


#define SUELDO_BASE 100
extern PCB ready_process;


#define SALARIO_POBRE 100
#define SALARIO_MEDIA 500
#define SALARIO_ALTA  5000

#define PRECIO_ALQUILER_BASE 10


// Estructuras del Scheduler
typedef struct {
    // Tus colas, bitmaps, árboles...
} RunQueue;


extern PCB ready_proc_vec[];
extern PCB waiting_proc_vec[];


ErrorCode init_scheduler(int tick_freq);
void scheduler();

#endif // SCHEDULER_H

/*
el sched tiene un presupuesto default,
cada proceso tiene un presupuesto para alquilar la cpu
cada vez que un nuevo proceso entra durante su ejecucion su dinero disminuye

*/