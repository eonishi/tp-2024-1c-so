#ifndef INSTRUCCION_H_
#define INSTRUCCION_H_

#include <stdio.h>  
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/log.h>
#include "../../shared/include/comunicacion.h"
#include "../../shared/include/protocolo.h"

extern t_log* logger;
extern t_list* procesos_en_memoria;
extern int socket_cpu;

typedef struct {
    unsigned PID;
    t_list* instrucciones;
} t_InstrSet; // Estoy abierto a ponerle un mejor nombre 🤮

t_list* leer_archivo_instrucciones(char* path);
void crear_instr_set(char *path, unsigned PID);
char *get_instr_by_pc();
void enviar_instruccion_a_cpu(char *instruccion);
void recibir_solicitud_de_cpu();

#endif