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

t_log *logger;
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
void iniciar_proceso_en_memoria(int conexion);
void dispatch_proceso(int conexion);

void iterator(char* value);







#endif