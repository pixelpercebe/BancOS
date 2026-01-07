#include <machine/machine.h>
#include <errors.h>
#include <utils.h>
#include <os.h>
#include <sys_paths.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <machine/core.h>

ErrorCode init_machine_architecture(){
    // Inicialización específica de la arquitectura de la máquina
    if (init_cores_struct() != OK){
        printf("Error al inicializar estructuras de cores\n");
        exit(ERR_CORE_INIT);
    }
    return OK;
}

//todo cambiar uso de atoi por safe atoi donde sea necesario
int load_machine_config(char * filepath){
    //cargar configuracion de maquina desde archivo ini utils trim_ini() y parsear valores
    //void trim_ini(const char *filepath, ini_file_t *output, size_t max_length)
    ini_file_t *config_hmap = (ini_file_t *)malloc(sizeof(ini_file_t));
    char full_path[256];
    // VERIFICACIÓN: Siempre verifica si malloc falló
    if (config_hmap == NULL) {
        return ERR_CONFIG_FILE; // O manejar el error apropiadamente
    }
    
    snprintf(full_path, sizeof(full_path), "%s%s", CONFIG_PATH, filepath);
    trim_ini(full_path,config_hmap,500);

    ini_section_t * current_section= config_hmap->sections;
    while (current_section != NULL){
        char * name = current_section ->name;
        ini_entry_t * current_entry = current_section->entries;
        if (!strcmp(name, "machine")){
            while (current_entry != NULL){
                char* key = current_entry->key;
                char* value = current_entry->value;
                //error
                if (key == NULL && value== NULL){
                    return ERR_CONFIG_FILE;
                }

                if(!strcmp(key, FIELD_NUM_CPU)){
                    bancos.machine_data.num_cpu = atoi(value);
                    VERBOSE_PRINTF("\nnum cpu cargado:%d", bancos.machine_data.num_cpu);
                    if (bancos.machine_data.num_cpu <= 0){
                        printf("\nError: num_cpu debe ser mayor que 0\n");
                        return ERR_CONFIG_FILE;
                    }
                }
                if(!strcmp(key,FIELD_CORES)){
                    bancos.machine_data.cpu_num_cores = atoi(value);
                    VERBOSE_PRINTF("\nnum cores por cpu cargado:%d", bancos.machine_data.cpu_num_cores);
                    if (bancos.machine_data.cpu_num_cores <= 0){
                        printf("\nError: cpu_num_cores debe ser mayor que 0\n");
                        return ERR_CONFIG_FILE;
                    }
                }
                if(!strcmp(key,FIELD_CLOCK_SPEED)){
                    bancos.machine_data.cpu_clock_speed_Ghz = atof(value);
                    VERBOSE_PRINTF("\nclock speed cargado:%f", bancos.machine_data.cpu_clock_speed_Ghz);
                    if (bancos.machine_data.cpu_clock_speed_Ghz <= 0){
                        printf("\nError: cpu_clock_speed_Ghz debe ser mayor que 0\n");
                        return ERR_CONFIG_FILE;
                    }
                }
                if(!strcmp(key,FIELD_THREADS)){
                    bancos.machine_data.cpu_hardware_threads = atoi(value);
                    VERBOSE_PRINTF("\nnum threads por cpu cargado:%d", bancos.machine_data.cpu_hardware_threads);
                    if (bancos.machine_data.cpu_hardware_threads <= 0){
                        printf("\nError: cpu_hardware_threads debe ser mayor que 0\n");
                        return ERR_CONFIG_FILE;
                    }
                }
                if(!strcmp(key,FIELD_CACHE_L1)){
                    bancos.machine_data.cpu_cache_L1 = atoi(value);
                    VERBOSE_PRINTF("\ncache L1 cargado:%d", bancos.machine_data.cpu_cache_L1);
                    if (bancos.machine_data.cpu_cache_L1 <= 0){
                        printf("\nError: cpu_cache_L1 debe ser mayor que 0\n");
                        return ERR_CONFIG_FILE;
                    }
                }
                if(!strcmp(key,FIELD_CACHE_L2)){
                    bancos.machine_data.cpu_cache_L2 = atoi(value);
                    VERBOSE_PRINTF("\ncache L2 cargado:%d", bancos.machine_data.cpu_cache_L2);
                    if (bancos.machine_data.cpu_cache_L2 <= 0){
                        printf("\nError: cpu_cache_L2 debe ser mayor que 0\n");
                        return ERR_CONFIG_FILE;
                    }
                }
                if(!strcmp(key,FIELD_CACHE_L3)){
                    bancos.machine_data.cpu_cache_L3 = atoi(value);
                    VERBOSE_PRINTF("\ncache L3 cargado:%d", bancos.machine_data.cpu_cache_L3);
                    if (bancos.machine_data.cpu_cache_L3 <= 0){
                        printf("\nError: cpu_cache_L3 debe ser mayor que 0\n");
                        return ERR_CONFIG_FILE;
                    }
                }
                current_entry = current_entry->next;
            }
        }
        else if (!strcmp(name, "scheduler")){
            while (current_entry != NULL){
                // parse scheduler entries here as needed
                char* key = current_entry->key;
                char* value = current_entry->value;

                if (key == NULL && value== NULL){
                    return ERR_CONFIG_FILE;
                }

                if(!strcmp(key,FIELD_SCHED_TICK_FREQ)){
                    bancos.machine_data.scheduler_tick_freq = atoi(value);
                    VERBOSE_PRINTF("\nscheduler tick freq cargado:%d", bancos.machine_data.scheduler_tick_freq);
                }
                if (!strcmp(key,FIELD_REPLACEMENT_POLICY)){
                    bancos.machine_data.replacement_policy = strdup(value);
                    VERBOSE_PRINTF("\nreplacement policy cargado:%s\n", bancos.machine_data.replacement_policy);
                }
                if(!strcmp(key,FIELD_GRANULARITY)){
                    bancos.bucket_cgs_granularity = atoi(value);
                    VERBOSE_PRINTF("\nbucket cgs granularity cargado:%d", bancos.bucket_cgs_granularity);
                    if (bancos.bucket_cgs_granularity <= 0){
                        printf("\nError: bucket_cgs_granularity debe ser mayor que 0\n");
                        return ERR_CONFIG_FILE;
                    }
                }
                if (!strcmp(key,FIELD_MAX_BUDGET)){
                    bancos.max_budget = atoi(value);
                    VERBOSE_PRINTF("\nmax budget cargado:%d", bancos.max_budget);
                    if (bancos.max_budget <= 0){
                        printf("\nError: max_budget debe ser mayor que 0\n");
                        return ERR_CONFIG_FILE;
                    }
                }

                current_entry = current_entry->next;
            }
        }
        else {
            printf("ERROR - Formato de archivo config erroneo\n");
            return ERR_CONFIG_FILE;
        }
        current_section = current_section->next;
    } 
    bancos.machine_data.total_cores = bancos.machine_data.num_cpu * bancos.machine_data.cpu_num_cores;
    VERBOSE_PRINTF("\ntotal cores cargado:%d", bancos.machine_data.total_cores);
    bancos.machine_data.total_hardware_threads = bancos.machine_data.total_cores * bancos.machine_data.cpu_hardware_threads;
    VERBOSE_PRINTF("\ntotal hardware threads cargado:%d\n", bancos.machine_data.total_hardware_threads);
    return OK;
}

ErrorCode init_cores_struct(){
    VERBOSE_PRINTF("\nInicializando %d cores en total\n", bancos.machine_data.total_cores);
    bancos.cores = (Core *)malloc(sizeof(Core) * bancos.machine_data.total_cores);
    if (bancos.cores == NULL) {
        return ERR_MEMORY_INSUFFICIENT;
    }

    for (int i = 0; i < bancos.machine_data.total_cores; i++) {
        bancos.cores[i].core_id = i;
        bancos.cores[i].num_configured_threads = bancos.machine_data.cpu_hardware_threads;
        bancos.cores[i].threads = (HardwareThread *)malloc(sizeof(HardwareThread) * bancos.machine_data.cpu_hardware_threads);
        if (bancos.cores[i].threads == NULL) {
            return ERR_MEMORY_INSUFFICIENT;
        }

        for (int j = 0; j < bancos.machine_data.cpu_hardware_threads; j++) {
            bancos.cores[i].threads[j].thread_id = j;
            bancos.cores[i].threads[j].current_process = NULL;
            bancos.cores[i].threads[j].is_idle = 1; // Inicialmente inactivo
        }
        //bancos.cores[i].current_rent_price = BASE_RENTAL_PRICE; // Precio base inicial
        // Lo rellena el scheduler cuando inicie
        bancos.cores[i].should_work = 0;
        bancos.cores[i].run_queue = NULL;       // Todavía no hay cola
        bancos.cores[i].current_rent_price = 0; // Todavía no hay precio definido
        bancos.cores[i].force_eviction = 0; // Inicializar bandera de interrupción a 0

        // Inicializar mutexes y condiciones
        // el bloqueo es una capacidad del core
        if (pthread_mutex_init(&bancos.cores[i].lock, NULL) != 0) {
            perror("Error al inicializar el mutex del core.");
            return ERR_MUTEX_INIT;
        }
        if (pthread_cond_init(&bancos.cores[i].wake_cond, NULL) != 0) {
            perror("Error al inicializar la condición del core.");
            return ERR_COND_INIT;
        }
    }
    return OK;
}