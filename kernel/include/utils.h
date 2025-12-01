#ifndef UTILS_H
#define UTILS_H

#include <types.h>

//todo crear struct para manejar archivos ini como hashmaps
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
void trim_ini(const char *filepath, char *output, size_t max_length);
#endif // UTILS_H