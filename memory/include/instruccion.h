#ifndef INSTRUCCION_H_
#define INSTRUCCION_H_

#include <stdio.h>  
#include <commons/collections/list.h>
#include <commons/string.h>
#include "../../shared/include/logger.h"

extern t_log* logger;
extern t_list* procesos_en_memoria;

typedef struct {
    unsigned PID;
    t_list* instrucciones;
} t_InstrSet; // Estoy abierto a ponerle un mejor nombre 🤮

t_list* leer_archivo_instrucciones(char* path);
void crear_instr_set(char *path, unsigned PID);
void *get_instr_by_pc(unsigned PID, unsigned PC);

#endif