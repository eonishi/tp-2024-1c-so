#ifndef CONEXION_IO_H_
#define CONEXION_IO_H_

#include "configuracion.h"
#include"../../shared/include/client.h"
#include"../../shared/include/comunicacion.h"
#include"../../shared/include/protocolo.h"
#include <commons/string.h>

extern t_log *logger;
extern io_config config;


void crear_conexion_kernel();
void crear_conexion_memoria();
int conectar_al_kernel(char *nombre_interfaz, io_tipo tipo);

#endif