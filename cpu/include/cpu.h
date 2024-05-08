#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "configuracion.h"
#include "conexion.h"
#include "ciclo.h"

t_log *logger;
extern int socket_memoria, socket_kernel;
extern cpu_config config;

void terminar_programa();

#endif