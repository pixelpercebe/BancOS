#ifndef SYNC_RESOURCES_H
#define SYNC_RESOURCES_H

#include <pthread.h>
#include <types.h>

// Variables de sincronización
extern pthread_mutex_t mutex;
extern pthread_cond_t cond;
extern pthread_cond_t cond2;

// Variables de estado compartidas
extern volatile u_llong global_ticks;
extern volatile int done;

#endif /* SYNC_RESOURCES_H */