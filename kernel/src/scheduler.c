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

static ErrorCode get_next_process(Core *core, PCB **out_process){
    // Implementar la lógica para obtener el siguiente proceso de la runqueue del core
    // Basado en la política de scheduling (CGS)
    // Por ahora, simplemente devolvemos NULL
    *out_process = NULL;
    return OK;

}

static ErrorCode get_process_bucket(PCB *process){
    if (process == NULL) return -1;
    int bucket = process->budget / bancos.bucket_cgs_granularity;
    return bucket;
}


static ErrorCode add_process_to_runqueue(PCB *process){
    RunQueue *rq = NULL;
    if (process->last_core != NULL && process->last_core > 0 && process->last_core < bancos.machine_data.total_cores) {
        Core *core = &bancos.cores[process->last_core];
        rq = core->run_queue;
    } else {
        for (int i = 0; i < bancos.machine_data.total_cores; i++) {
            continue;
            //todo buscar core con menos procesos
            }
        }
    

    //todo verificar que esto sea correcto
    ///////////////////////////////////////////
    if (rq == NULL || process == NULL) return ERR_INVALID_PARAMETER;

    int bucket_index = get_process_bucket(process);
    if (bucket_index < 0 || bucket_index >= rq->num_buckets) {
        return ERR_INVALID_PARAMETER;
    }

    Bucket *bucket = &rq->buckets[bucket_index];

    // Añadir el proceso al final del bucket
    if (bucket->head == NULL) {
        // Bucket vacío
        bucket->head = process;
        bucket->tail = process;
        process->next_br = NULL;
        process->prev_br = NULL;
    } else {
        // Bucket no vacío
        bucket->tail->next_br = process;
        process->prev_br = bucket->tail;
        process->next_br = NULL;
        bucket->tail = process;
    }

    // Actualizar la runqueue
    rq->count++;

    // Actualizar el bitmap de activos
    int array_idx = bucket_index / 32;
    int bit_pos = bucket_index % 32;
    rq->active_bitmap[array_idx] |= (1 << bit_pos);

    // Actualizar el máximo bucket activo si es necesario
    if (bucket_index > rq->max_active_bucket) {
        rq->max_active_bucket = bucket_index;
    }

    ///////////////////////////////////////////

    return OK;
}

static void print_info()
{
    printf("\n [SCHED] ejecución del scheduler global\n");
}


/**
 * @brief Marca un bit en el mapa de bits como activo.
 
static void set_bitmap_bit(RunQueue *rq, int index) {
    int array_idx = index / 32;
    int bit_pos = index % 32;
    rq->active_bitmap[array_idx] |= (1 << bit_pos);
}*/

/**
 * @brief Desmarca un bit si el bucket se vació.
 
static void clear_bitmap_bit(RunQueue *rq, int index) {
    int array_idx = index / 32;
    int bit_pos = index % 32;
    rq->active_bitmap[array_idx] &= ~(1 << bit_pos);
}*/


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

    printf("[SCHED] Inicializando HASH MAP Dinámico:\n");
    printf("   - Presupuesto Max: %d\n", max_money);
    printf("   - Granularidad:    %d (1 bucket = %d euros)\n", gran, gran);
    printf("   - Total Buckets:   %d\n", num_buckets);

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

        int bitmap_size = (num_buckets / 32) + 1;; // Número de enteros necesarios para el bitmap
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
    while (1) {
        // 1. ESPERAR LA SEÑAL DEL RELOJ (DORMIR)
        pthread_mutex_lock(&mi_core->lock);
        while (mi_core->should_work == 0) {
            pthread_cond_wait(&mi_core->wake_cond, &mi_core->lock);
        }
        pthread_mutex_unlock(&mi_core->lock);

        
        // 2. SELECCIÓN (Pick Next)
        // 3. DISPATCH (Mudanza)
        // 4. EJECUCIÓN CON LIMITES (Runtime Dinámico)
            // a) Pagar Impuestos
            // b) Envejecer
        // 5. LIQUIDACIÓN
        printf("[Core %d] Procesando ciclo...\n", mi_core->core_id);
        //gestionar_vivienda(mi_core); 
        dispatcher(mi_core);



        // 6. VOLVER A DORMIR
        pthread_mutex_lock(&mi_core->lock);
        mi_core->should_work = 0; // Me vuelvo a dormir hasta el siguiente aviso
        pthread_mutex_unlock(&mi_core->lock);
    }
    return NULL;
}

ErrorCode dispatcher(Core *core) {
    // Implementar la lógica del despachador aquí
    // Esta función debería seleccionar un nuevo proceso para ejecutar en el core
    // basándose en la política de scheduling y el estado de la runqueue del core

    printf("[Core %d] Dispatcher invocado.\n", core->core_id);

    // Ejemplo simple: seleccionar el proceso más rico de la runqueue
    PCB *next_process = NULL;
    if (next_process != NULL) {
        printf("[Core %d] Seleccionando proceso PID: %d para ejecución.\n", core->core_id, next_process->pid);
        // Aquí se implementaría la lógica para asignar el proceso al core
    } else {
        printf("[Core %d] No hay procesos disponibles en la runqueue.\n", core->core_id);
    }

    return OK;
}


