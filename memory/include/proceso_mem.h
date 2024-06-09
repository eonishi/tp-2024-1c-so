#ifndef PROCESO_MEM_H_
#define PROCESO_MEM_H_

#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include "tabla_paginas.h"
#include "configuracion.h"

extern t_log* logger;
extern memory_config config;

typedef struct {
    unsigned PID;
    t_list* instrucciones;
    t_dictionary* tabla_paginas;
} t_proceso_en_memoria;

void inicializar_procesos_en_memoria();
void cargar_proceso_en_memoria(char *path, unsigned PID);
void liberar_proceso_en_memoria(unsigned PID);
bool memoria_tiene_pid(void* set_instrucciones, unsigned PID);
bool memoria_tiene_pid_solicitado(void* set_instrucciones);

#endif