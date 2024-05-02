#ifndef INSTRUCCION_H_
#define INSTRUCCION_H_

#include <stdio.h>  
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h> //Posible primera implementacion para almacenar los procesos en memoria (?)
#include <commons/string.h>
#include "../../shared/include/logger.h"

extern t_log* logger;

typedef struct {
    unsigned PID;
    t_list* instrucciones;
} t_InstrSet; // Estoy abierto a ponerle un mejor nombre 🤮

t_list* leer_archivo_instrucciones(char* path);
t_InstrSet *crear_instr_set(char *path, unsigned PID);

#endif