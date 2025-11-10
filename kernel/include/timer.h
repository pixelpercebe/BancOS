#ifndef TIMER_H
#define TIMER_H
#include <errors.h>

#define MAX_TIMERS 10
#define TIMER_ACTIVE 1
#define TIMER_INACTIVE 0

typedef struct timer
{
    int timer_id;
    int duration;
    int is_active;
    void (*callback)(void);
} Timer;

extern int last_id;

Timer timers_list[MAX_TIMERS]; //todo borrar lista de timers

ErrorCode add_timer(int duration);
ErrorCode remove_timer(int timer_id);
ErrorCode activate_timer(int timer_id);
ErrorCode deactivate_timer(int timer_id);

ErrorCode init_timer_module();

//void set_num_timers(int n);
#endif // TIMER_H