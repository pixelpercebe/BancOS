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
 * @brief Imprime la lista global de procesos.
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

/**
 * @brief Muestra los procesos actualmente en ejecución por core/hilo.
 */
void print_running_processes() {
    printf("\n=== PROCESOS EN EJECUCIÓN (CPU) ===\n");
    printf("%-6s | %-6s | %-6s | %-10s | %-10s\n", "Core", "Hilo", "PID", "Budget", "Clase");
    printf("------------------------------------------------------\n");

    int total_running = 0;

    for (int i = 0; i < bancos.machine_data.total_cores; i++) {
        Core *c = &bancos.cores[i];
        
        for (int t = 0; t < c->num_configured_threads; t++) {
            PCB *p = c->threads[t].current_process;
            
            if (p != NULL) {
                // Convertir enum clase a texto para que se vea bonito
                const char* class_names[] = {"VAGABUNDO", "BAJA", "MEDIA", "ALTA", "ELITE"};
                const char* c_name = (p->class >= 0 && p->class <= 4) ? class_names[p->class] : "UNKNOWN";

                printf("  %2d   |   %2d   |  %4d  | %10d | %s\n", 
                       c->core_id, t, p->pid, p->budget, c_name);
                total_running++;
            }
        }
    }

    if (total_running == 0) {
        printf("  (Ningún proceso en ejecución - Sistema IDLE)\n");
    }
    printf("======================================================\n");
}

/**
 * @brief Muestra el estado de colas y carga por core.
 */
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

/**
 * @brief Hilo de consola interactiva.
 * @param arg No utilizado.
 * @return Siempre NULL.
 */
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
            printf("  ps   - Listar TODOS los procesos (Global List)\n");
            printf("  run  - Ver solo procesos en EJECUCIÓN (CPU)\n"); // <--- Nuevo
            printf("  qs   - Ver estado de colas y carga\n");
            printf("  exit - Apagar el simulador\n");
        }
        else if (strcmp(buffer, "ps") == 0) {
            print_all_processes(); 
        } 
        else if (strcmp(buffer, "qs") == 0) {
            print_queues();
        } 
        else if (strcmp(buffer, "run") == 0) {
            print_running_processes();
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


