#ifndef CLOCK_H
#define CLOCK_H

typedef struct {
    int seconds;
    int nanoseconds;
} Time;

int init_clock_module(float freq_cpu);
void set_num_timers(int n);
int get_global_ticks();
#endif