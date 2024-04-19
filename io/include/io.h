#ifndef IO_H_
#define IO_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include "../../shared/include/logger.h"
#include"../../shared/include/client.h"

t_log *logger;

typedef struct{
    char *IP_KERNEL;
    char *PUERTO_KERNEL;
    char *IP_MEMORIA;
    char *PUERTO_MEMORIA;
    //char *TIPO_INTERFAZ;
    //unsigned TIEMPO_UNIDAD_TRABAJO;
    //char *PATH_BASE_DIALFS;
    //unsigned BLOCK_SIZE;
    //unsigned BLOCK_COUNT;
    //unsigned RETRASO_COMPACTACION;
} io_config;

io_config config;

int crear_conexion_kernel();

#endif