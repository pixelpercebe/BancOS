
#include <task_map.h>
#include <timer.h>
#include <errors.h>

Timer task_map[TASK_LIST_LENGTH];

ErrorCode add_task(int task_id, Timer timer) {
    if (task_id < 0 || task_id >= TASK_LIST_LENGTH) {
        return ERR_INVALID_TASK_ID;
    }
    task_map[task_id] = timer;
    return OK;
}

ErrorCode get_task_timer(int task_id, Timer *timer){
    if (task_id < 0 || task_id >= TASK_LIST_LENGTH) {
        return ERR_INVALID_TASK_ID;
    }
    *timer = task_map[task_id];
    return OK;
}

ErrorCode activate_task(int task_id){
    if (task_id < 0 || task_id >= TASK_LIST_LENGTH) {
        return ERR_INVALID_TASK_ID;
    }
    task_map[task_id].is_active = TIMER_ACTIVE;
    return OK;
}
ErrorCode deactivate_task(int task_id){
    if (task_id < 0 || task_id >= TASK_LIST_LENGTH) {
        return ERR_INVALID_TASK_ID;
    }
    task_map[task_id].is_active = TIMER_INACTIVE;
    return OK;
}

void print_task_map() {
    for (int i = 0; i < TASK_LIST_LENGTH; i++) {
        Timer timer = task_map[i];
        printf("Task ID: %d, Timer ID: %d, Duration: %d, Active: %d\n",
               i, timer.timer_id, timer.duration, timer.is_active);
    }
}