#ifndef ERRORS_H
#define ERRORS_H

typedef enum {
    OK = 0, 
    ERR_ARGS = 1,
    ERR_CLOCK_INIT,
    ERR_TIMER_INIT,
    ERR_MEMORY_INSUFFICIENT,
    ERR_INVALID_PARAMETER,
    ERR_INVALID_TASK_ID,
    ERR_CONFIG_FILE,
    ERR_THREAD_CREATE,
    ERR_THREAD_JOIN,
    ERR_MUTEX_INIT,
    ERR_COND_INIT,
    ERR_CORE_INIT,
    ERR_SCHEDULER_INIT,
    ERR_MACHINE_ARCH_INIT,
    ERR_INVALID_PROCESS,
    ERR_INVALID_BUCKET,
    ERR_EMPTY_RUNQUEUE,
    
    ABORT,
    
    // Un error final para saber el total de errores si es necesario
    MAX_ERRORS
} ErrorCode;
#endif // ERRORS_H