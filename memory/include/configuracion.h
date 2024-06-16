#ifndef CONFIGURACION_MEMORIA_H_
#define CONFIGURACION_MEMORIA_H_

#include <unistd.h>
#include <commons/log.h>
#include <commons/config.h>

extern t_log* logger;

typedef struct
{
    char *ip_memoria;
    char *puerto_memoria;
    unsigned tam_memoria;
    unsigned tam_pagina;
    char *path_instrucciones;
    unsigned retardo_memoria;
} memory_config;

void inicializar_configuracion();
void esperar_retardo();

#endif