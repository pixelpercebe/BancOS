#include <sync_resources.h>
#include <types.h>

// Sincronización de funciones base del sistema operativo
// se mantienen en este archivo debido a que es estrictamente necesario
// que funcionen en orden para que el sistema operativo funcione correctamente.
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;

volatile u_llong global_ticks = 0;
volatile int done = 0;