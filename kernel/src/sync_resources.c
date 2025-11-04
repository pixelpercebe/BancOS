#include "sync_resources.h"
#include <pthread.h>

// Definición (e inicialización) de las variables
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;

volatile u_llong global_ticks = 0;
volatile int done = 0;