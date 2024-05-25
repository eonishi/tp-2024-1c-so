#ifndef GESTOR_IO_H_
#define GESTOR_IO_H_

#include <commons/log.h>
#include <commons/collections/queue.h>
#include <stdlib.h>
#include <stdio.h>
#include "conexion.h"

bool validar_y_enviar_instruccion_a_io();
bool existe_io_conectada(char* nombre_io);
conexion_io* obtener_conexion_io_por_nombre(char* nombre_io);
bool io_acepta_operacion(conexion_io conexion_io,char* operacion_io);
bool io_disponible(conexion_io conexion_io);

#endif