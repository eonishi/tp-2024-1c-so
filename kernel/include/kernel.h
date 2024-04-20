#ifndef KERNEL_H_
#define KERNEL_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <commons/collections/list.h>
#include "../../shared/include/logger.h"
#include "../../shared/include/client.h"
#include "../../shared/include/server.h"

t_log *logger;
typedef struct
{
    char *ip_kernel;
    char *puerto_kernel;
    char *ip_memoria;
    char *puerto_memoria;
    char *ip_cpu;
    char *puerto_cpu;
} kernel_config;


kernel_config config;

void inicializar_configuracion();
int crear_conexion_cpu();
int crear_conexion_memoria();


void iterator(char* value);







#endif