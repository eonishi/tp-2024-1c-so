#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include "../../shared/include/logger.h"
#include "../../shared/include/server.h"

t_log *logger;

typedef struct
{
    char *ip_cpu;
    char *puerto_cpu;
    char *ip_memoria;
    char *puerto_memoria;
} cpu_config;


cpu_config config;

void inicializar_configuracion();
void iterator(char* value);

#endif