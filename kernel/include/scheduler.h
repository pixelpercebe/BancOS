#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <types.h>
#include <errors.h>

ErrorCode init_scheduler(int tick_freq);
void scheduler();

#endif // SCHEDULER_H