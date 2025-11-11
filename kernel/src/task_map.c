
#include <OS_task/task_map.h>
#include <stdio.h>
#include <timer.h>
#include <errors.h>

Timer task_map[TASK_LIST_LENGTH];
int number_of_tasks = 0;


/**
 * @brief Añade una tarea al mapa de tareas.
 * @param task_id: ID de la tarea.
 * @param timer: Estructura Timer asociada a la tarea.
 * @return: Código de error.
 */
ErrorCode add_task(int task_id, Timer timer) {
    if (task_id < 0 || task_id >= TASK_LIST_LENGTH) {
        return ERR_INVALID_TASK_ID;
    }
    task_map[task_id] = timer;
    number_of_tasks++;
    return OK;
}

/**
 * @brief Obtiene el temporizador asociado a una tarea.
 * @param task_id: ID de la tarea.
 * @param timer: Puntero a la estructura Timer donde se almacenará el temporizador.
 * @return: Código de error.
 */
ErrorCode get_task_timer(int task_id, Timer *timer){
    if (task_id < 0 || task_id >= TASK_LIST_LENGTH) {
        return ERR_INVALID_TASK_ID;
    }
    *timer = task_map[task_id];
    return OK;
}


/**
 * @brief Activa una tarea en el mapa de tareas.
 * @param task_id: ID de la tarea.
 * @return: Código de error.
 */
ErrorCode activate_task(int task_id){
    if (task_id < 0 || task_id >= TASK_LIST_LENGTH) {
        return ERR_INVALID_TASK_ID;
    }
    task_map[task_id].is_active = TIMER_ACTIVE;
    return OK;
}

/**
 * @brief Desactiva una tarea en el mapa de tareas.
 * @param task_id: ID de la tarea.
 * @return: Código de error.
 */
ErrorCode deactivate_task(int task_id){
    if (task_id < 0 || task_id >= TASK_LIST_LENGTH) {
        return ERR_INVALID_TASK_ID;
    }
    task_map[task_id].is_active = TIMER_INACTIVE;
    return OK;
}


/**
 * @brief Imprime el mapa de tareas.
 */
void print_task_map() {
    for (int i = 0; i < TASK_LIST_LENGTH; i++) {
        Timer timer = task_map[i];
        printf("Task ID: %d, Timer ID: %d, Duration: %d, Active: %d\n",
               i, timer.timer_id, timer.duration, timer.is_active);
    }
}