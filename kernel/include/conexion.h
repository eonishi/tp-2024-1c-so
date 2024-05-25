#ifndef CONEXION_KERNEL_H_
#define CONEXION_KERNEL_H_

#include <commons/log.h>
#include <stdlib.h>
#include "configuracion.h"
#include "../../shared/include/client.h"
#include "../../shared/include/comunicacion.h"
#include "../../shared/include/server.h"

extern t_log *logger;
extern kernel_config* config;
extern int socket_cpu_dispatch, socket_cpu_interrupt, socket_memoria, socket_io;;

bool generar_conexiones();
int generar_conexion(char* ip, char* puerto);
int recibir_conexion_io(int server);

#endif