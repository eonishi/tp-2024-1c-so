#ifndef CONEXION_IO_H_
#define CONEXION_IO_H_

#include <stdlib.h>
#include "configuracion.h"
#include <commons/string.h>
#include <commons/log.h>
#include"../../shared/include/client.h"
#include"../../shared/include/comunicacion.h"
#include"../../shared/include/protocolo.h"

extern t_log *logger;
extern io_config config;

void crear_conexion_kernel();
void crear_conexion_memoria();
int conectar_al_kernel(char *nombre_interfaz, io_tipo tipo, int* instrucciones);

#endif