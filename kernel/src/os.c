
#include <os.h>
#include <stdio.h>
#include <machine/machine.h>
#include <pcb.h>
#include <timer.h>
#include <utils.h>
#include <sys_paths.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

system_t bancos;

void system_init(){
    printf("Sistema inicializado\n");
    load_machine_config();
}

int load_machine_config(char * filepath){
    //todo: cargar configuracion de maquina desde archivo ini utils trim_ini() y parsear valores
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
                    printf("\nnum cpu cargado:%d", bancos.machine_data.num_cpu);
                }
                if(!strcmp(key,FIELD_CORES)){
                    bancos.machine_data.cpu_num_cores = atoi(value);
                    printf("\nnum cores por cpu cargado:%d", bancos.machine_data.cpu_num_cores);
                }
                if(!strcmp(key,FIELD_CLOCK_SPEED)){
                    bancos.machine_data.cpu_clock_speed_Ghz = strtoul(value,NULL,10);
                    printf("\nclock speed cargado:%lu", bancos.machine_data.cpu_clock_speed_Ghz);
                }
                if(!strcmp(key,FIELD_THREADS)){
                    bancos.machine_data.cpu_hardware_threads = atoi(value);
                    printf("\nnum threads por cpu cargado:%d", bancos.machine_data.cpu_hardware_threads);
                }
                if(!strcmp(key,FIELD_CACHE_L1)){
                    bancos.machine_data.cpu_cache_L1 = atoi(value);
                    printf("\ncache L1 cargado:%d", bancos.machine_data.cpu_cache_L1);
                }
                if(!strcmp(key,FIELD_CACHE_L2)){
                    bancos.machine_data.cpu_cache_L2 = atoi(value);
                    printf("\ncache L2 cargado:%d", bancos.machine_data.cpu_cache_L2);
                }
                if(!strcmp(key,FIELD_CACHE_L3)){
                    bancos.machine_data.cpu_cache_L3 = atoi(value);
                    printf("\ncache L3 cargado:%d", bancos.machine_data.cpu_cache_L3);
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
                    printf("\nscheduler tick freq cargado:%d", bancos.machine_data.scheduler_tick_freq);
                }
                if (!strcmp(key,FIELD_REPLACEMENT_POLICY)){
                    bancos.machine_data.replacement_policy = strdup(value);
                    printf("\nreplacement policy cargado:%s\n", bancos.machine_data.replacement_policy);
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
    return OK;
}