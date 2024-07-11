#ifndef PROCESO_MEM_H_
#define PROCESO_MEM_H_

#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include "configuracion.h"

extern t_log* logger;
extern memory_config config;

typedef struct {
    unsigned PID;
    t_list* instrucciones;
    t_list *tabla_paginas;
} t_proceso_en_memoria;

void inicializar_procesos_en_memoria();
void cargar_proceso_en_memoria(char *path, unsigned PID);
void liberar_proceso_en_memoria(unsigned PID);
bool memoria_tiene_pid(void* set_instrucciones, unsigned PID);
bool memoria_tiene_pid_solicitado(void* set_instrucciones);
t_proceso_en_memoria *get_proceso_by_PID(unsigned PID, unsigned *PID_ptr, bool memoria_tiene_pid(void *));

// Esta viene de tabla_paginas.h
extern void quitar_paginas(size_t cantidad_de_paginas, t_proceso_en_memoria* proceso);

#endif