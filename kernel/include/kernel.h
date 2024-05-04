#ifndef KERNEL_H_
#define KERNEL_H_

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<commons/collections/list.h>
#include "configuracion.h"
#include "conexion.h"
#include "../../shared/include/logger.h"
#include "../../shared/include/client.h"
#include "../../shared/include/server.h"
#include "../../shared/include/comunicacion.h"
#include "../../shared/include/pcb.h"

t_log *logger;
kernel_config* config;
int pcb_counter = 1;
int socket_cpu, socket_memoria;

t_list* procesoNew;
t_list* procesoReady;
t_list* procesoExecute;
t_list* procesoBlock;
t_list* procesoExit;

typedef enum
{
	CPU,
	MEMORIA,
} cod_mensaje;

void terminar_programa();
void iniciar_consola();
void *iniciar_escucha();
void iniciar_servidor_en_hilo();
pcb* iniciar_proceso_en_memoria();
void dispatch_proceso(pcb* pcb);






#endif