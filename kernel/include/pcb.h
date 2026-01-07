#ifndef PCB_H
#define PCB_H

typedef enum SocialClass{
    VAGABUNDO,
    BAJA,
    MEDIA,
    ALTA,
    ELITE
} SocialClass;

typedef enum ProcessState{
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} ProcessState;

// Definición de la estructura PCB
typedef struct PCB{
    int pid;                // ID del proceso
    int lifetime;
    int final_tick;
    SocialClass class;

    int last_core;      // id Último core donde se ejecutó
    int last_thread;    // id Último hilo donde se ejecutó
    ProcessState state;          // Estado del proceso (READY, RUNNING, BLOCKED, etc.)

    int budget;
    struct PCB *next_br;  // Puntero al siguiente PCB en la lista de todos los procesos 
    struct PCB *prev_br;  // Puntero al PCB anterior en la lista de todos los procesos 

    struct PCB *next_pcb_runqueue; //proximo puntero en la runqueu del bucket
    struct PCB *prev_pcb_runqueue; //proximo puntero en la runqueu del bucket
    
    //todo jerarquia de procesos
    // puntero a pcb hijos
    //PCB * first_child;
    //puntero a pcb padre
    //PCB * parent;
} PCB;

#endif