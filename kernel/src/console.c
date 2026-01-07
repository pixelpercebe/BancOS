#include <console.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <os.h>
#include <machine/core.h>
#include <machine/machine.h>
#include <unistd.h>

// Si no lo tienes en un .h global, defínelo aquí para evitar errores de compilación
#ifndef INFLATION_DIVIDER
#define INFLATION_DIVIDER 10 
#endif


/**
 * @brief Imprime la lista de procesos generados.
 */
static void print_all_processes() {
    pthread_mutex_lock(&bancos.list_pcb_lock);
    
    PCB *current = bancos.list_pcb_head;
    printf("\n--- LISTA GLOBAL DE PROCESOS (%d activos) ---\n", bancos.process_count);
    
    while (current != NULL) {
        printf("PID: %d | Budget: %d | Estado: %d\n", current->pid, current->budget, current->state);
        current = current->next_br;
    }
    
    pthread_mutex_unlock(&bancos.list_pcb_lock);
}

static void print_queues() {
    printf("\n=== ESTADO DE LAS COLAS ===\n");
    for (int i = 0; i < bancos.machine_data.total_cores; i++) {
        Core *c = &bancos.cores[i];
        
        // Acceso seguro (lectura sucia permitida para monitorización)
        int load = (c->run_queue) ? c->run_queue->count : 0;
        int active = 0;
        
        // Contar activos en CPU
        for(int t=0; t < c->num_configured_threads; t++) {
            if(c->threads[t].current_process != NULL) active++;
        }
        load += active;
        int inflacion = (load > 0) ? (load / INFLATION_DIVIDER) : 1;
        if (inflacion < 1) inflacion = 1;

        printf("Core %d: CPU [%d/%d] | Cola [%d] | Inflación [x%d]\n", 
               c->core_id, active, c->num_configured_threads, load, inflacion);
    }
    printf("===========================\n");
}

void *console_routine(void *arg) {
    char buffer[100];
    
    // Esperamos un poco para que el log inicial del sistema no se mezcle
    usleep(100000); 

    printf("\n[CONSOLA] Iniciada. Escribe 'help' para comandos.\n");

    while (1) {
        printf("\n<FONDOS_OS> ");
        fflush(stdout); // Asegura que el prompt salga antes de esperar input

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;

        // Eliminar salto de línea
        buffer[strcspn(buffer, "\n")] = 0;

        if (strlen(buffer) == 0) continue; // Ignorar enter vacío

        if (strcmp(buffer, "help") == 0) {
            printf("Comandos disponibles:\n");
            printf("  ps   - Listar todos los procesos (Global List)\n");
            printf("  qs   - Ver estado de colas y carga (RunQueues)\n");
            printf("  exit - Apagar el simulador\n");
        } 
        else if (strcmp(buffer, "ps") == 0) {
            print_all_processes(); 
        } 
        else if (strcmp(buffer, "qs") == 0) {
            print_queues();
        } 
        else if (strcmp(buffer, "exit") == 0) {
            printf("Apagando sistema...\n");
            exit(0);
        }
        else {
            printf("Comando desconocido: '%s'. Escribe 'help'.\n", buffer);
        }
    }
    return NULL;
}


