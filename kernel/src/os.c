
#include <os.h>
#include <stdio.h>
#include <machine/machine.h>
#include <pcb.h>
#include <timer.h>
#include <utils.h>
#include <paths.h>

system_t bancos;

void system_init(){
    printf("Sistema inicializado\n");
}

static void load_machine_config(){
    //todo: cargar configuracion de maquina desde archivo ini utils trim_ini() y parsear valores
}