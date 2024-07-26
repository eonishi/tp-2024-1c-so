#ifndef GESTOR_IO_H_
#define GESTOR_IO_H_


#include <semaphore.h>
#include <pthread.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "conexion.h"
#include "colas_planificador.h"
#include "detener_planificacion.h"
#include "../../shared/include/pcb.h"
#include "../../shared/include/protocolo.h"
#include "../../shared/include/direccion.h"
#include "commons/temporal.h"

extern t_log *logger;
extern t_log *aux_log;
extern sem_t sem_proceso_en_ready;
extern t_list *lista_conexiones_io;
extern t_temporal *q_transcurrido;
extern pthread_mutex_t mutex_conexiones_io;

void *escuchar_io(void *socket);

bool validar_instruccion_a_io(char** instruc_io_tokenizadas, pcb* pcb);
bool existe_io_conectada(char* nombre_io);
conexion_io* obtener_conexion_io_por_nombre(char* nombre_io);
bool io_acepta_operacion(conexion_io conexion_io,char* operacion_io);
void enviar_proceso_a_esperar_io(pcb* pcb_a_espera);
void cerrar_conexion_io(char *nombre_io);

#endif