#ifndef GESTOR_IO_H_
#define GESTOR_IO_H_


#include <semaphore.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "conexion.h"
#include "colas_planificador.h"
#include "../../shared/include/pcb.h"
#include "../../shared/include/protocolo.h"

extern t_log *logger;
extern sem_t sem_proceso_en_ready;
extern t_list *lista_conexiones_io;

void *escuchar_io(void *socket);

bool validar_y_enviar_instruccion_a_io(char** instruc_io_tokenizadas, t_list* peticiones_memoria);
bool existe_io_conectada(char* nombre_io);
conexion_io* obtener_conexion_io_por_nombre(char* nombre_io);
bool io_acepta_operacion(conexion_io conexion_io,char* operacion_io);
bool io_disponible(conexion_io conexion_io);

#endif