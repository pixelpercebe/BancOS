#include <process_generator.h>

#include <stdio.h>
#include <stdlib.h>

#include <pcb.h>
#include <clock.h>

//TODO quiza pueda ir aqui un timer_process_generator?

void generate_process(PCB process[]){
    PCB new_process;
    new_process.pid = rand() % 1000; // Generar un PID aleatorio
    new_process.lifetime = (rand() % 10) + 1; // Duración entre 1 y 10 ticks
    new_process.final_tick = get_current_global_ticks() + new_process.lifetime;
    process [new_process.pid] = new_process;
    printf("Proceso generado: PID=%d, Lifetime=%d, Final Tick=%d\n", new_process.pid, new_process.lifetime, new_process.final_tick);
}
