#ifndef INSTRUCCION_H_
#define INSTRUCCION_H_

#include <commons/collections/list.h>
#include <commons/log.h>
#include "proceso_mem.h"
#include "../../shared/include/comunicacion.h"
#include "../../shared/include/protocolo.h"

extern t_log* logger;
extern int socket_cpu;
extern unsigned PID_solicitado;
extern t_list *procesos_en_memoria;

void enviar_instruccion_a_cpu();
void recibir_fetch_de_cpu();

#endif