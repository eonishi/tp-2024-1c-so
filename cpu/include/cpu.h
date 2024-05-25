#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "configuracion.h"
#include "conexion.h"
#include "ciclo.h"
#include "../../shared/include/logger.h"

t_log *logger;
extern int socket_memoria, socket_kernel;
extern cpu_config config;
extern pcb *pcb_actual;

void terminar_programa();

#endif