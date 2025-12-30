
#include <os.h>
#include <stdio.h>
#include <machine/machine.h>
#include <pcb.h>
#include <timer.h>
#include <utils.h>
#include <sys_paths.h>
#include <errors.h>
#include <machine/core.h>
#include <scheduler.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

system_t bancos;

void system_init(){
    if (init_scheduler(bancos.machine_data.scheduler_tick_freq) != OK) {
        printf("Error al inicializar el scheduler\n");
        exit(ERR_SCHEDULER_INIT);
    }
}