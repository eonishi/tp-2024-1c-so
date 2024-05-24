#ifndef CONEXION_IO_H_
#define CONEXION_IO_H_

#include <commons/log.h>
#include <stdlib.h>
#include "configuracion.h"
#include "../../shared/include/client.h"
#include "../../shared/include/comunicacion.h"

extern t_log *logger;
extern io_config *config;
extern int socket_kernel;

bool generar_conexiones();
int generar_conexion(char* ip, char* puerto);

#endif