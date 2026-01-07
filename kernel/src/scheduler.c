#include <scheduler.h>

#include <os.h>
#include <stdlib.h>
#include <errors.h>
#include <types.h>
#include <OS_task/task_def.h>
#include <OS_task/task_map.h>
#include <stdio.h>
#include <timer.h>
#include <pthread.h>
#include <pcb.h>
#include <machine/machine.h>
#include <machine/core.h>
#include <sync_resources.h>

/*********************************************************************************
 *                  FUNCIONES AUXILIARES DEL BITMAP DE RUNQUEUE                  *
 *********************************************************************************
*/
/**
 * @brief Marca un bit en el mapa de bits como activo.
 * @param rq Puntero a la runqueue.
 * @param index Índice global del bucket a marcar.
 */
 
static void set_bitmap_bit(RunQueue *rq, int index) {
    int array_idx = index / 32;
    int bit_pos = index % 32;
    rq->active_bitmap[array_idx] |= (1 << bit_pos);
}

/**
 * @brief Desmarca un bit si el bucket se vació.
 * @param rq Puntero a la runqueue.
 * @param index Índice global del bucket a desmarcar.
 */
static void clear_bitmap_bit(RunQueue *rq, int index) {
    int array_idx = index / 32;
    int bit_pos = index % 32;
    rq->active_bitmap[array_idx] &= ~(1 << bit_pos);
}

/*********************************************************************************
 *                  FUNCIONES AUXILIARES DEL SCHEDULER                          *
 *********************************************************************************
*/

/**
 * @brief Devuelve el bucket activo de mayor prioridad.
 * @param rq Puntero a la runqueue.
 * @return Índice del bucket o -1 si no hay buckets activos.
 */
int get_highest_priority_bucket(RunQueue *rq) {
    // 1. Calcular cuántas palabras (enteros) tiene tu bitmap
    int num_words = (rq->num_buckets + (BITS_PER_WORD - 1)) / BITS_PER_WORD;

    // 2. Recorremos las palabras de ARRIBA a ABAJO
    for (int i = num_words - 1; i >= 0; i--) {
        
        u_long word = rq->active_bitmap[i];

        // 3. Si la palabra no es cero, ¡aquí está el ganador!
        if (word != 0) {
            
            // __builtin_clz cuenta cuántos ceros hay a la izquierda antes del primer 1.
            // Ejemplo en 32 bits: 0000...0100 -> 29 ceros.
            // El índice del bit es: 31 - ceros.
            int leading_zeros = __builtin_clz(word);
            int bit_index = (BITS_PER_WORD - 1) - leading_zeros;

            // 4. Calculamos el índice global del bucket
            // (Índice de palabra * 32) + índice del bit dentro de la palabra
            int global_bucket_index = (i * BITS_PER_WORD) + bit_index;

            // Protección extra: asegurarse de no salirnos del rango real de buckets
            if (global_bucket_index >= rq->num_buckets) {
                continue; // Fue un bit fantasma fuera de rango (raro si limpias bien)
            }

            return global_bucket_index;
        }
    }

    return -1; // La cola está completamente vacía
}

/**
 * @brief Obtiene el siguiente proceso de la runqueue del core.
 * @param core Core objetivo.
 * @param out_process Salida con el proceso seleccionado (o NULL si vacío).
 * @return `OK` o un código de error si ocurre inconsistencia.
 */
static ErrorCode get_next_process(Core *core, PCB **out_process){
    // Implementar la lógica para obtener el siguiente proceso de la runqueue del core
    // Basado en la política de scheduling (CGS)
    // Por ahora, simplemente devolvemos NULL

    if (core == NULL || out_process == NULL) {
        return ERR_INVALID_PARAMETER;
    }
    *out_process = NULL;
    RunQueue *rq = core->run_queue;

    if (rq->count == 0) return OK; // No hay procesos en la runqueue


    int found_index = get_highest_priority_bucket(rq);
    
    if (found_index == -1) {
        //count > 0 pero no encontramos a nadie. Inconsistencia.
        return ERR_EMPTY_RUNQUEUE; 
    }
    Bucket *bucket = &rq->buckets[found_index];
    PCB *selected = bucket->head;

    bucket->head = selected->next_pcb_runqueue;
    if (bucket->head == NULL) {
        bucket->tail = NULL;
        clear_bitmap_bit(rq, found_index);
    }

    selected->next_pcb_runqueue = NULL;
    selected->prev_pcb_runqueue = NULL;

    rq->count--;
    *out_process = selected;

    
    return OK;

}

/**
 * @brief Calcula el índice de bucket para un proceso según su presupuesto.
 * @param process Proceso a evaluar.
 * @return Índice de bucket o `ERR_INVALID_PROCESS`.
 */
static inline ErrorCode get_process_bucket(PCB *process){
    if (process == NULL) return ERR_INVALID_PROCESS;
    int bucket = process->budget / bancos.bucket_cgs_granularity;
    return bucket;
}

/**
 * @brief Calcula la carga actual del core (cola + procesos en CPU).
 * @param c Puntero al core.
 * @return Carga total.
 */
int get_core_load(Core *c) {
    int active_threads = 0;
    for (int i = 0; i < c->num_configured_threads; i++) {
        if (c->threads[i].current_process != NULL) {
            active_threads++;
        }
    }
    return c->run_queue->count + active_threads;
}

/**
 * @brief Selecciona el core con menor carga.
 * @param out_core Core seleccionado (salida).
 * @return `OK` o `ERR_INVALID_PARAMETER` si no hay cores.
 */
ErrorCode get_valid_core(Core **out_core){
    if (bancos.machine_data.total_cores == 0) return ERR_INVALID_PARAMETER;

    Core *best_core = &bancos.cores[0];
    u_int min_load = get_core_load(best_core);

    for(int i = 0; i < bancos.machine_data.total_cores; i++) {
        int current_load = get_core_load(&bancos.cores[i]);
        if (current_load < min_load) {
            min_load = current_load;
            best_core = &bancos.cores[i];
        }
    }
    *out_core = best_core;
    return OK;
}


/***********************************************************************
 *                  FUNCIONES PARA LAS RUNQUEUES                       *
 ************************************************************************
*/

/**
 * @brief Encola un proceso en la runqueue del core según clase y presupuesto.
 * @param core Core de destino.
 * @param process Proceso a encolar.
 * @return `OK` o error (`ERR_INVALID_PARAMETER`, `ERR_INVALID_BUCKET`).
 */
static ErrorCode add_process_to_runque(Core *core, PCB *process){

    if (core == NULL || process == NULL) return ERR_INVALID_PARAMETER;

    process->budget += INITIAL_SALARY[process->class];
    if (process->budget > bancos.max_budget) {
        process->budget = bancos.max_budget;
    }

    RunQueue *rq = core->run_queue;
    int bucket_index = get_process_bucket(process);

    if (bucket_index < 0 || bucket_index >= rq->num_buckets) return ERR_INVALID_BUCKET;

    Bucket *bucket = &rq->buckets[bucket_index];

    // Añadir el proceso al final del bucket
    if (bucket->head == NULL) {
        bucket->head = process;
        bucket->tail = process;
        process->next_pcb_runqueue = NULL;

        // Marcar el bucket como activo en el bitmap
        set_bitmap_bit(rq, bucket_index);

        if (bucket_index > rq->max_active_bucket) {
            rq->max_active_bucket = bucket_index;
        }

    } 
    else { //lista no vacia
        if (process->class == ELITE) {
            // Proceso ELITE va al frente
            process->next_pcb_runqueue = bucket->head;
            bucket->head = process;
            } 
        else {
            // Proceso normal va al final
            bucket->tail->next_pcb_runqueue = process;
            bucket->tail = process;
            process->next_pcb_runqueue = NULL;
        }
}
    rq->count++;
    process->last_core = core->core_id;
    VERBOSE_PRINTF("[Core %d] Proceso PID: %d añadido al bucket %d de la runqueue.\n", core->core_id, process->pid, bucket_index);

    return OK;
}


/**
 * @brief Elimina un proceso de las estructuras y libera su memoria.
 * @param proc Proceso a liberar.
 */
void free_pcb(PCB *proc) {
    if (proc == NULL) return;

    remove_global_pcb(proc);
    // PASO 1: Liberar memoria interna
    proc->next_br = NULL;
    proc->prev_br = NULL;
    proc->next_pcb_runqueue = NULL;
    proc->prev_pcb_runqueue = NULL;



    // PASO 2: Liberar la estructura principal
    // Esto devuelve el bloque de memoria del struct PCB al sistema operativo.
    free(proc);
}

/**
 * @brief Admite un proceso en el core con protección de mutex y señalización.
 * @param core Core objetivo.
 * @param process Proceso a admitir.
 * @return `OK` o un código de error al encolar.
 */
ErrorCode scheduler_admit_process(Core *core, PCB *process) {
    if (!core || !process) return ERR_INVALID_PARAMETER;

    VERBOSE_PRINTF("[Core %d] Solicitud de admisión para PID: %d.\n", core->core_id, process->pid);

    // 1. SEGURIDAD: Bloqueamos la puerta (Mutex)
    // Esto es lo que la diferencia de la otra función.
    pthread_mutex_lock(&core->lock);

    // 2. MECÁNICA: Llamamos a la función interna
    // Como tenemos el lock, es seguro tocar los punteros.

    core->force_eviction = 1; // Indicamos que hay una interrupción pendiente
    process->state = READY; // READY
    process->last_core = core->core_id;

    ErrorCode err = add_process_to_runque(core, process);

    if (err != OK) {
        pthread_mutex_unlock(&core->lock); // Importante liberar si fallamos
        return err;
    }

    // 4. NOTIFICACIÓN: Tocar el timbre
    if (core->should_work == 0) {
        core->should_work = 1;
        pthread_cond_signal(&core->wake_cond);
    }

    // desbloqueamos el mutex
    pthread_mutex_unlock(&core->lock);

    return OK;
}

/**
 * @brief Traza informativa del scheduler global por tick (solo verbose).
 */
static void print_info()
{
    VERBOSE_PRINTF("\n [SCHED] ejecución del scheduler global\n");
}

/***********************************************************************
 *                  FUNCIONES de los cores                             *
 ***********************************************************************
*/


/**
 * @brief Busca el índice del hilo candidato para ser desalojado.
 * @return int: Índice del hilo (0..N) o -1 si no se debe desalojar a nadie.
 */
static int get_eviction_victim_index(Core *core) {
    if (core == NULL) return -1;

    int victim_idx = -1;
    
    // Inicializamos con los valores más altos posibles para ir bajando
    SocialClass lowest_class = ELITE; 
    int lowest_budget = 2147483647; // INT_MAX

    for (int i = 0; i < core->num_configured_threads; i++) {
        PCB *p = core->threads[i].current_process;

        //Hay un hueco libre
        // Si encontramos un hilo vacío, NO necesitamos víctima.
        // Devolvemos -1 para indicar el uso del hueco libre.
        if (p == NULL) {
            return -1; 
        }

        //Evaluar si es el más "pobre" encontrado hasta ahora
        int is_worse = 0;

        //si Clase Social más baja
        if (p->class < lowest_class) {
            is_worse = 1;
        } 
        // si la clase es igual
        else if (p->class == lowest_class) {
            if (p->budget < lowest_budget) {
                is_worse = 1;
            }
        }

        // Actualizar candidato
        if (is_worse) {
            lowest_class = p->class;
            lowest_budget = p->budget;
            victim_idx = i;
        }
    }

    return victim_idx;
}


/*-------------------------------------------------------------------------------*
 |                        FUNCIONES SCHEDULER PRINCIPALES                        |
 *-------------------------------------------------------------------------------*/


/**
 * @brief Inicializa el módulo del scheduler global y locales.
 * @param tick_freq: Frecuencia de ticks del scheduler.
 * @return: Código de error.
 */
ErrorCode init_scheduler(int tick_freq)
{
    u_int max_money = (bancos.max_budget > 0) ? bancos.max_budget : 30000;
    u_int gran = (bancos.bucket_cgs_granularity > 0) ? bancos.bucket_cgs_granularity : 10;
    u_int num_buckets = (max_money/gran) + 1;

    VERBOSE_PRINTF("[SCHED] Inicializando HASH MAP Dinámico:\n");
    VERBOSE_PRINTF("   - Presupuesto Max: %d\n", max_money);
    VERBOSE_PRINTF("   - Granularidad:    %d (1 bucket = %d euros)\n", gran, gran);
    VERBOSE_PRINTF("   - Total Buckets:   %d\n", num_buckets);

    // Crear hilos de scheduler local para cada core
    for (int i = 0; i < bancos.machine_data.total_cores; i++) {
        Core *core = &bancos.cores[i];
        core->run_queue = (RunQueue *)malloc(sizeof(RunQueue));
        if (core->run_queue == NULL) return ERR_MEMORY_INSUFFICIENT;

        ////////////////////////////////////////////////////////////////

        core->run_queue->count = 0;
        core->run_queue->num_buckets = num_buckets;
        core->run_queue->max_active_bucket = -1;


        core->run_queue->buckets = (Bucket *)calloc(num_buckets, sizeof(Bucket));
        if (core->run_queue->buckets == NULL) {
            free(core->run_queue);
            return ERR_MEMORY_INSUFFICIENT;
        }

        u_long bitmap_size = (num_buckets / 32) + 1;; // Número de enteros necesarios para el bitmap
        core->run_queue->bitmap_size = bitmap_size;

        core->run_queue->active_bitmap = (u_int *) calloc(bitmap_size, sizeof(u_int));
        if (core->run_queue->active_bitmap == NULL) {
            free(core->run_queue->buckets);
            free(core->run_queue);
            return ERR_MEMORY_INSUFFICIENT;
        }

        ////////////////////////////////////////////////////////////////

        if (pthread_create(&core->thread_id, NULL, local_core_scheduler, (void *)core) != 0) {
            perror("Error al crear el hilo del core.");
            return  ERR_THREAD_CREATE;
        }
    }

    int ret = init_timer_module(tick_freq, TIMER_ACTIVE, scheduler, &bancos.task_map[SCHE]);
    if (ret == ERR_TIMER_INIT) exit(ERR_TIMER_INIT);
    print_task_map();
    return OK;
}



/**
 * @brief Función del scheduler.
 */
void scheduler()
{
    print_info();
    for (int i = 0; i < bancos.machine_data.total_cores; i++) {
        Core *core = &bancos.cores[i];
        // Despertar el core para que trabaje en este tick
        pthread_mutex_lock(&core->lock);
        core->should_work = 1;
        pthread_cond_signal(&core->wake_cond);
        pthread_mutex_unlock(&core->lock);
    }
}



// esta es la rutina de scheduler local de cada core, se llamara desde el task scheduler 
// que se ha guardado en el task map
// cada core tiene su propia runqueue y su propio scheduler local
// el scheduler local se encargara de seleccionar el siguiente proceso a ejecutar en ese core
// basandose en la politica de scheduling definida (CGS en este caso)
// y en el estado de la runqueue de ese core

/**
 * @brief Rutina del núcleo trabajador del scheduler.
 * @param arg: Argumento pasado a la rutina.
 * @return: Puntero nulo.
 */
void * local_core_scheduler(void *arg)
{
    Core *mi_core = (Core *)arg;
    
    // Bloqueamos ANTES de entrar al bucle (Modelo Síncrono)
    pthread_mutex_lock(&mi_core->lock);

    while (1) {
        // 1. ESPERA PASIVA
        while (mi_core->should_work == 0) {
            pthread_cond_wait(&mi_core->wake_cond, &mi_core->lock);
        }

        VERBOSE_PRINTF("[Core %d] Iniciando Tick...\n", mi_core->core_id);

        // 2. CÁLCULO DE INFLACIÓN (Dentro del lock y en cada tick)
        // Protegemos la lectura de run_queue->count
        u_int inflation_factor = 1;
        if (mi_core->run_queue->count > 0) {
            inflation_factor = mi_core->run_queue->count / INFLATION_DIVIDER;
        }
        if (inflation_factor < 1) inflation_factor = 1;
        VERBOSE_PRINTF("[Core %d] Factor de inflación calculado: %d\n", mi_core->core_id, inflation_factor);

        int target_victim_idx = -1;

        // Si hay orden de desalojo, buscamos a quién echar
        if (mi_core->force_eviction == 1) {
            target_victim_idx = get_eviction_victim_index(mi_core);
    
            if (target_victim_idx != -1) {
                VERBOSE_PRINTF("[Core %d] Objetivo marcado: Hilo %d será desalojado.\n", mi_core->core_id, target_victim_idx);
            }
        }

        // Iteramos sobre los hilos de hardware (SMT)
        for (int i = 0; i < mi_core->num_configured_threads; i++) {
            
            PCB *proc = mi_core->threads[i].current_process;
            int slot_is_empty = (proc == NULL);

            // --- FASE A: GESTIÓN DEL PROCESO EXISTENTE ---
            if (!slot_is_empty) {
                
                // a) Cobrar Impuestos (Fórmula de Coste)
                // Asegúrate de que TAX_PER_TICK sea accesible aquí
                int tax = TAX_PER_TICK[proc->class] + (QUANTUM_BASE[proc->class] * inflation_factor);
                proc->budget -= tax;
                
                // b) Envejecer
                // Asumo que restas lifetime también, o usas global_ticks para comparar
                
                // c) Verificar Estados de Terminación
                int must_leave_cpu = 0;

                // C1. Muerte por Vejez
                if (global_ticks >= proc->final_tick) {
                          VERBOSE_PRINTF("[Core %d][Hilo %d]  PID %d terminó (Time Limit).\n", 
                           mi_core->core_id, i, proc->pid);
                    free_pcb(proc);
                    must_leave_cpu = 1;
                }
                // C2. Muerte por Bancarrota (Budget <= 0) -> READY
                else if (proc->budget <= 0) {
                          VERBOSE_PRINTF("[Core %d][Hilo %d]  PID %d en BANCARROTA.\n", 
                           mi_core->core_id, i, proc->pid);
                    proc->state = READY;
                    add_process_to_runque(mi_core, proc); // Devolver a la cola
                    must_leave_cpu = 1;
                }
                // C3. Expulsión por Interrupción (Preemption) -> READY
                else if (mi_core->force_eviction == 1 && i == target_victim_idx) {
                    if (proc->class )
                          VERBOSE_PRINTF("[Core %d][Hilo %d]  PID %d desalojado (Preemption).\n", 
                           mi_core->core_id, i, proc->pid);
                    
                    proc->state = READY;
                    add_process_to_runque(mi_core, proc); // Devolver a la cola
                    must_leave_cpu = 1;
                }
                else {
                    // Sigue vivo y ejecutando
                          VERBOSE_PRINTF("[Core %d][Hilo %d] PID %d ejecutando. Budget restante: %d\n", 
                           mi_core->core_id, i, proc->pid, proc->budget);
                }

                // Si murió o se fue, limpiamos el puntero local
                if (must_leave_cpu) {
                    mi_core->threads[i].current_process = NULL;
                    slot_is_empty = 1; // Marcamos como vacío para la Fase B
                    mi_core->threads[i].is_idle = 1;
                    mi_core->force_eviction = 0; // Limpiar la bandera de expulsión
                }
            }

            // --- FASE B: RELLENAR HUECOS (Scheduling) ---
            // Si el slot está vacío (porque lo estaba o porque el proceso acaba de morir/salir)
            // intentamos llenarlo AHORA MISMO.
            if (slot_is_empty) {
                VERBOSE_PRINTF("hilo vacio en core %d hilo %d\n", mi_core->core_id, i);
                PCB *new_proc = NULL;
                
                // Llamamos a tu función de selección (Pick Next)
                // Nota: Asegúrate de que get_next_process busque en la RunQueue correcta
                get_next_process(mi_core, &new_proc); 

                if (new_proc != NULL) {
                          VERBOSE_PRINTF("[Core %d][Hilo %d] Seleccionado PID %d para ejecución.\n", 
                           mi_core->core_id, i, new_proc->pid);
                    new_proc->state = RUNNING;
                    new_proc-> last_thread = i;
                    mi_core->threads[i].current_process = new_proc;
                          VERBOSE_PRINTF("[Core %d][Hilo %d] PID %d entra a CPU.\n", 
                           mi_core->core_id, i, new_proc->pid);
                } else {
                    // printf("[Core %d][Hilo %d] IDLE (Nada en cola).\n", mi_core->core_id, i);
                }
            }
        } // Fin del for de hilos

        // Limpieza de banderas post-tick 
        mi_core->force_eviction = 0;
        mi_core->should_work = 0; // Apagar luz y esperar siguiente señal
    }
    
    pthread_mutex_unlock(&mi_core->lock);
    return NULL;
}


