#ifndef PCB_H
#define PCB_H

typedef enum SocialClass{
    VAGABUNDO,
    BAJA,
    MEDIA,
    ALTA,
    ELITE
} SocialClass;

// Definición de la estructura PCB
typedef struct PCB{
    int pid;                // ID del proceso
    int lifetime;
    int final_tick;
    SocialClass class;

    int presupuesto;
    int last_core;      // Último core donde se ejecutó
    int last_thread;    // Último hilo donde se ejecutó
    int estado;         // Estado del proceso (READY, RUNNING, BLOCKED, etc.)

    struct PCB *next_br;  // Puntero al siguiente PCB en la lista
    struct PCB *prev_br;  // Puntero al PCB anterior en la lista
    
    //todo jerarquia de procesos
    // puntero a pcb hijos
    //PCB * first_child;
    //puntero a pcb padre
    //PCB * parent;
} PCB;

#endif