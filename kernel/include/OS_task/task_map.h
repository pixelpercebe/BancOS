
#ifndef OS_TASK_TASK_MAP_H
#define OS_TASK_TASK_MAP_H
#include <timer.h>
#include <errors.h>


#define TASK_LIST_LENGTH 16

extern Timer task_map[TASK_LIST_LENGTH];
extern int number_of_tasks;

ErrorCode add_task(int task_id, Timer timer);
ErrorCode get_task_timer(int task_id, Timer *timer);
ErrorCode activate_task(int task_id);
ErrorCode deactivate_task(int task_id);

void print_task_map();

#endif // OS_TASK_TASK_MAP_H