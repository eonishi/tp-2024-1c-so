#ifndef CONEXION_KERNEL_H_
#define CONEXION_KERNEL_H_

#include <commons/log.h>
#include <stdlib.h>
#include "configuracion.h"
#include "../../shared/include/client.h"
#include "../../shared/include/comunicacion.h"

extern t_log *logger;
extern kernel_config* config;

bool generar_conexiones(int* socket_cpu, int* socket_memoria);
int generar_conexion(char* ip, char* puerto);

#endif