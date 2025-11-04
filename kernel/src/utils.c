#include <utils.h>
#include <types.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <errors.h>

/*
 * Calcula la duración en microsegundos de cada tick basada en segundos y nanosegundos.
 */
void calculate_microseconds(int seconds, int nanoseconds, u_llong *microseconds) {
    *microseconds = (u_llong)(seconds * 1e6 + nanoseconds / 1e3);
}