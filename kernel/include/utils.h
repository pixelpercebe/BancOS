#ifndef UTILS_H
#define UTILS_H

#include <types.h>
#include <stdbool.h>

//ini configurable fields
#define FIELD_NUM_CPU "num_cpu"
#define FIELD_CORES "cpu_num_cores"
#define FIELD_CLOCK_SPEED "cpu_clock_speed_Ghz"
#define FIELD_THREADS "cpu_hardware_threads"
#define FIELD_CACHE_L1 "cpu_cache_L1"
#define FIELD_CACHE_L2 "cpu_cache_L2"
#define FIELD_CACHE_L3 "cpu_cache_L3"
#define FIELD_REPLACEMENT_POLICY "replacement_policy"
#define FIELD_SCHED_TICK_FREQ "scheduler_tick_freq"
#define FIELD_GRANULARITY "bucket_cgs_granularity"
#define FIELD_MAX_BUDGET "max_budget"


typedef struct ini_entry {
    char *key;
    char *value;
    struct ini_entry *next;
} ini_entry_t;

// Estructura para una Sección (Colección de Entradas)
typedef struct ini_section {
    char *name;     // Ej: "MachineConfig"
    ini_entry_t *entries; // Puntero a la primera entrada clave-valor de esta sección
    struct ini_section *next; // Puntero a la siguiente sección en el archivo
} ini_section_t;

// Estructura para el archivo INI completo
typedef struct {
    ini_section_t *sections; // Puntero a la primera sección
} ini_file_t;

void calculate_microseconds(int seconds, int nanoseconds, u_llong *microseconds);
void trim_ini(const char *filepath, ini_file_t *output, size_t max_length);
void print_array(void*array, size_t element_size, size_t length);

// Funciones de conversión segura
bool safe_atoi(const char *str, int *result);
bool safe_atof(const char *str, float *result);
bool safe_atod(const char *str, double *result);
bool safe_atoui(const char *str, u_int *result);

#endif // UTILS_H