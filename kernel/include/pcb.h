#ifndef PCB_H
#define PCB_H

typedef enum{
    VAGABUNDO,
    BAJA,
    MEDIA,
    ALTA,
    ELITE
} ClaseSocial;

// Definición de la estructura PCB
typedef struct {
    int pid;                // ID del proceso
    int lifetime;
    int final_tick;
    ClaseSocial clase;
    int presupuesto;
} PCB;

#endif