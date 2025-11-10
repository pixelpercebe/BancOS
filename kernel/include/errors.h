#ifndef ERRORS_H
#define ERRORS_H

typedef enum {
    OK = 0, 
    ERR_ARGS = 1,
    ERR_CLOCK_INIT,
    ERR_TIMER_INIT,
    ERR_MEMORY_INSUFFICIENT,
    ERR_INVALID_PARAMETER,
    
    // Un error final para saber el total de errores si es necesario
    MAX_ERRORS
} ErrorCode;
#endif // ERRORS_H