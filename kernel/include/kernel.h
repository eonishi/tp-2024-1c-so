#ifndef KERNEL_H_
#define KERNEL_H_

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <commons/collections/list.h>
#include "../../shared/include/logger.h"
#include "../../shared/include/client.h"
#include "../../shared/include/server.h"
#include "../../shared/include/comunicacion.h"
#include "../../shared/include/pcb.h"

t_log *logger;
int pcb_counter = 1;
int socket_cpu, socket_memoria;

typedef enum
{
	CPU,
	MEMORIA,
} cod_mensaje;

typedef struct
{
    char *ip_kernel;
    char *puerto_kernel;
    char *ip_memoria;
    char *puerto_memoria;
    char *ip_cpu;
    char *puerto_cpu;
} kernel_config;


kernel_config config;

void inicializar_configuracion();
int crear_conexion_cpu();
int crear_conexion_memoria();
void terminar_programa();
void iniciar_consola();
void *iniciar_escucha();
void iniciar_servidor_en_hilo();
pcb* iniciar_proceso_en_memoria();
void dispatch_proceso(pcb* pcb);






#endif