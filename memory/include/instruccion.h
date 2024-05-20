#ifndef INSTRUCCION_H_
#define INSTRUCCION_H_

#include <stdio.h>  
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/log.h>
#include "configuracion.h"
#include "../../shared/include/comunicacion.h"
#include "../../shared/include/protocolo.h"

extern t_log* logger;
extern t_list* procesos_en_memoria;
extern int socket_cpu;
extern memory_config config;

typedef struct {
    unsigned PID;
    t_list* instrucciones;
} t_InstrSet; // Estoy abierto a ponerle un mejor nombre 🤮

void crear_instr_set(char *path, unsigned PID);
void enviar_instruccion_a_cpu();
void recibir_solicitud_de_cpu();
void liberar_instr_set(unsigned PID);
void log_instrucciones(unsigned PID);

#endif