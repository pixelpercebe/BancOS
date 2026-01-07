#include <interruptions.h>
#include <pcb.h>
#include <machine/core.h>
#include <scheduler.h>
#include <errors.h>
#include <stdio.h>
#include <os.h>


/**
 * @brief Atiende la interrupción de llegada de un nuevo proceso.
 *
 * Selecciona un core objetivo (usando afinidad si existe) y solicita su admisión
 * al scheduler local correspondiente.
 * @param process Puntero al proceso recién creado.
 * @return `OK` si fue encolado correctamente, o un código de error.
 */
ErrorCode new_process_interruption(PCB *process) {
    VERBOSE_PRINTF("[Interrupción] Nuevo proceso PID: %d recibido.\n", process->pid);
    Core *target_core = NULL;
    
    if (process == NULL) {
        printf("[Interrupción] Error: Proceso nulo recibido.\n");
        return ERR_INVALID_PROCESS;
    }

    if (process ->last_core != -1){
        target_core = &bancos.cores[process ->last_core];
        VERBOSE_PRINTF("[Interrupción] Proceso PID: %d asignado a su último core: Core %d.\n", process->pid, target_core->core_id);
    }

    ErrorCode err = get_valid_core(&target_core);
    if (err != OK) {
        printf("[Interrupción] Error al agregar el proceso PID: %d a la runqueue del core %d.\n", process->pid, target_core->core_id);
        return err;
    }
    VERBOSE_PRINTF("[Interrupción] Core seleccionado para el proceso PID: %d es Core %d.\n", process->pid, target_core->core_id);

    // Despertar el core para que trabaje en este tick
    scheduler_admit_process(target_core, process);

    VERBOSE_PRINTF("[Interrupción] Proceso PID: %d agregado a la runqueue del core %d.\n", process->pid, target_core->core_id);
    return OK;
}

//add_process_to_runque(target_core, process);