#pragma once
#include <types.h>
#include <machine/machine.h>
#include <errors.h>
#include <pcb.h>

typedef struct {
    u_int thread_id;
    PCB *current_process;
    u_int is_idle;
    // Otros campos específicos de core.h
} HardwareThread;

// Estructura del Core (La "Vivienda")
typedef struct {
    int core_id;
    
    // --- GESTIÓN DE PARALELISMO ---
    pthread_t thread_id;       // El hilo padre (scheduler) del sistema operativo
    pthread_mutex_t lock;      // Mutex para sincronización del core
    pthread_cond_t wake_cond;  // Señal de "Despertador" (Tick)
    int should_work;           // Bandera: 1 = Trabaja, 0 = Duerme

    // Configuración económica local
    int current_rent_price; // Precio de alquiler actual del core
    
    // Recursos hardware (Hilos/Pisos)
    // Puntero para soportar número variable de hilos por core
    HardwareThread *threads;
    int num_configured_threads; // Número de hilos configurados en este core

    // Cola de espera local (RunQueue)
    // lista de los procesos esperando piso en ESTE core
    // (estruct RunQueue en scheduler.h)
    struct RunQueue *run_queue;
    int force_eviction; // Bandera para indicar si hay una interrupción pendiente
    
} Core;

// Función que ejecuta cada hilo
void * core_worker_routine(void *arg);