#ifndef CLOCK_H
#define CLOCK_H

struct Clock {
    void (*start)(float freq_ghz, int num_timers);
    int (*get_global_ticks)();
    void (*set_num_timers)(int n);
};


#endif