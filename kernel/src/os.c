
#include <os.h>
#include <stdio.h>
#include <machine/machine.h>
#include <pcb.h>
#include <timer.h>
#include <utils.h>
#include <sys_paths.h>
#include <errors.h>
#include <machine/core.h>
#include <scheduler.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

system_t bancos;

void system_init(){
    //inicializar mutex de la lista global de PCBs
    pthread_mutex_init(&bancos.list_pcb_lock, NULL);
    //inicializar contador de procesos
    bancos.process_count = 0;
    bancos.list_pcb_head = NULL;
    bancos.list_pcb_tail = NULL;

    if (init_scheduler(bancos.machine_data.scheduler_tick_freq) != OK) {
        printf("Error al inicializar el scheduler\n");
        exit(ERR_SCHEDULER_INIT);
    }
}


// Inserta al final de la lista global (O(1))
void insert_global_pcb(PCB *p) {
    if (!p) return;

    pthread_mutex_lock(&bancos.list_pcb_lock);

    p->next_br = NULL;
    p->prev_br = bancos.list_pcb_tail;

    if (bancos.list_pcb_tail) {
        bancos.list_pcb_tail->next_br = p;
    } else {
        bancos.list_pcb_head = p; // Era el primero
    }
    bancos.list_pcb_tail = p;
    bancos.process_count++;

    pthread_mutex_unlock(&bancos.list_pcb_lock);
}


// Elimina de la lista global (O(1)) Se llama antes de free()
void remove_global_pcb(PCB *p) {
    if (!p) return;

    pthread_mutex_lock(&bancos.list_pcb_lock);

    // Desenganchar del anterior
    if (p->prev_br) {
        p->prev_br->next_br = p->next_br;
    } else {
        bancos.list_pcb_head = p->next_br; // Era la cabeza
    }

    // Desenganchar del siguiente
    if (p->next_br) {
        p->next_br->prev_br = p->prev_br;
    } else {
        bancos.list_pcb_tail = p->prev_br; // Era la cola
    }

    bancos.process_count--;
    pthread_mutex_unlock(&bancos.list_pcb_lock);
}