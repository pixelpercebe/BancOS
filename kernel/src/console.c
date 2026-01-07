#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <os.h>
#include <machine/core.h>

// Prototipos de funciones de impresión que ya tienes o harás públicas
extern void print_all_processes(); // La que hicimos con la lista global

void print_queues() {
    VERBOSE_PRINTF("\n=== ESTADO DE LAS COLAS ===\n");
    for (int i = 0; i < bancos.machine_data.total_cores; i++) {
        Core *c = &bancos.cores[i];
        int load = c->run_queue->count;
        int active = 0;
        // Contar activos en CPU
        for(int t=0; t<c->num_configured_threads; t++) 
            if(c->threads[t].current_process) active++;

        VERBOSE_PRINTF("Core %d: CPU [%d/%d] | Cola [%d] | Inflación [%d]\n", 
               c->core_id, active, c->num_configured_threads, load, 
               (load > 0 ? load/INFLATION_DIVIDER : 1));
    }
}

void *console_routine(void *arg) {
    char buffer[100];
    
    VERBOSE_PRINTF("\n[CONSOLA] Iniciada. Escribe 'help' para comandos.\n");

    while (1) {
        VERBOSE_PRINTF("\nFONDOS_OS> ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;

        // Eliminar salto de línea
        buffer[strcspn(buffer, "\n")] = 0;

        if (strcmp(buffer, "help") == 0) {
            VERBOSE_PRINTF("Comandos: ps (Procesos), qs (Colas), exit\n");
        } 
        else if (strcmp(buffer, "ps") == 0) {
            // ¡IMPORTANTE! Las funciones de impresión deben usar sus propios mutex
            print_all_processes(); 
        } 
        else if (strcmp(buffer, "qs") == 0) {
            print_queues();
        } 
        else if (strcmp(buffer, "exit") == 0) {
            VERBOSE_PRINTF("Cerrando sistema...\n");
            exit(0);
        }
    }
    return NULL;
}