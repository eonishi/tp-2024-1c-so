#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include "../../shared/include/logger.h"
#include "../../shared/include/server.h"

t_log* logger;

typedef struct
{
    char *ip_memoria;
    char *puerto_memoria;
} memory_config;


memory_config config;

#endif