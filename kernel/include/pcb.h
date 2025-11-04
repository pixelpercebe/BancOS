#ifndef PCB_H
#define PCB_H

// Definición de la estructura PCB
typedef struct {
    int pid;                // ID del proceso
    int lifetime;
    int final_tick;
} PCB;

#endif