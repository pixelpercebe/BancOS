#include <utils.h>
#include <types.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <errors.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
/*
 * Calcula la duración en microsegundos de cada tick basada en segundos y nanosegundos.
 */
void calculate_microseconds(int seconds, int nanoseconds, u_llong *microseconds) {
    *microseconds = (u_llong)(seconds * 1e6 + nanoseconds / 1e3);
}

void delete_white_spaces(char *str, char* output) {
    char *read_ptr = str;
    char *write_ptr = output;

    while (*read_ptr) {
        if (*read_ptr != ' ' && *read_ptr != '\t' && *read_ptr != '\n' && *read_ptr != '\r') {
            *write_ptr++ = *read_ptr;
        }
        read_ptr++;
    }
    *write_ptr = '\0';

}

/**
 * funcion generica para separarar datos de cualquier archivo ini
 * @param filepath ruta del archivo ini
 * @param output buffer donde se guardara la informacion leida
 * @param max_length tamaño maximo del buffer de salida
 * 
 */
void trim_ini(const char *filepath, ini_file_t *output, size_t max_length) {
    //todo : implementar funcion para leer archivos ini y parsear valores
    printf("\npath: %s",filepath);
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        perror("UTILS_ERR Error al abrir el archivo");
        return;
    }

    char line[256];
    ini_section_t *current_section = NULL;

    while (fgets(line,sizeof(line),file)){
        delete_white_spaces(line,line);
        // Procesar la línea sin espacios en blanco
        
        if (line[0] == '[') {
            // Nueva sección
            ini_section_t *new_section = (ini_section_t *)malloc(sizeof(ini_section_t));
            new_section->name = strdup(line + 1); // Saltar el '['
            char *closing_bracket = strchr(new_section->name, ']');
            if (closing_bracket) {
                *closing_bracket = '\0'; // Terminar el nombre de la sección
            }
            new_section->entries = NULL;
            new_section->next = NULL;

            if (current_section == NULL) {
                output->sections = new_section;
            } else {
                current_section->next = new_section;
            }
            current_section = new_section;
        } else if (strchr(line, '=')) {
            // Entrada clave-valor
            char *equals_sign = strchr(line, '=');
            *equals_sign = '\0';
            char *key = strdup(line);
            char *value = strdup(equals_sign + 1);

            ini_entry_t *new_entry = (ini_entry_t *)malloc(sizeof(ini_entry_t));
            new_entry->key = key;
            new_entry->value = value;

            // Agregar la nueva entrada a la sección actual
            if (current_section != NULL) {
                new_entry->next = current_section->entries;
                current_section->entries = new_entry;
            }
        }
    }

}

void print_array(void*array, size_t element_size, size_t length) {
    uint8_t *byte_array = (uint8_t *)array;
    for (size_t i = 0; i < length; i++) {
        for (size_t j = 0; j < element_size; j++) {
            printf("%02X ", byte_array[i * element_size + j]);
        }
        
    }
    printf("\n");
}

/**
 * @brief Convierte string a entero de forma segura
 * @param str: String a convertir
 * @param result: Puntero donde guardar el resultado
 * @return: true si la conversión fue exitosa, false en caso contrario
 */
bool safe_atoi(const char *str, int *result) {
    if (str == NULL || *str == '\0') return false;
    
    char *endptr;
    errno = 0;
    long val = strtol(str, &endptr, 10);
    
    // Verificar errores de conversión
    if (errno == ERANGE || val > INT_MAX || val < INT_MIN) return false;
    if (endptr == str || *endptr != '\0') return false;
    
    *result = (int)val;
    return true;
}

/**
 * @brief Convierte string a float de forma segura
 * @param str: String a convertir
 * @param result: Puntero donde guardar el resultado
 * @return: true si la conversión fue exitosa, false en caso contrario
 */
bool safe_atof(const char *str, float *result) {
    if (str == NULL || *str == '\0') return false;
    
    char *endptr;
    errno = 0;
    double val = strtof(str, &endptr);
    
    // Verificar errores de conversión
    if (errno == ERANGE || isnan(val) || isinf(val)) return false;
    if (endptr == str || *endptr != '\0') return false;
    
    *result = (float)val;
    return true;
}

/**
 * @brief Convierte string a double de forma segura
 * @param str: String a convertir
 * @param result: Puntero donde guardar el resultado
 * @return: true si la conversión fue exitosa, false en caso contrario
 */
bool safe_atod(const char *str, double *result) {
    if (str == NULL || *str == '\0') return false;
    
    char *endptr;
    errno = 0;
    double val = strtod(str, &endptr);
    
    // Verificar errores de conversión
    if (errno == ERANGE || isnan(val) || isinf(val)) return false;
    if (endptr == str || *endptr != '\0') return false;
    
    *result = val;
    return true;
}