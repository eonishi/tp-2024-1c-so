#ifndef IO_H_
#define IO_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include "configuracion.h"
#include "conexion.h"
#include "../../shared/include/logger.h"
#include"../../shared/include/client.h"
#include"../../shared/include/comunicacion.h"
#include"../../shared/include/protocolo.h"

t_log *logger;
io_config *config;
int socket_kernel;

void iniciar_escucha_kernel();

void terminar_programa(int conexion_kernel);

#endif