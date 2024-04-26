#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
#include "../../shared/include/logger.h"
#include "../../shared/include/server.h"
#include "../../shared/include/client.h"
#include "../../shared/include/comunicacion.h"

t_log* logger;

int socket_cpu, socket_kernel;

pthread_t hilo_cpu;
pthread_t hilo_kernel;

typedef struct
{
    char *ip_memoria;
    char *puerto_memoria;
} memory_config;

void iterator(char* value);

memory_config config;

void inicializar_configuracion();
void iniciar_gestor_solicitudes_en_hilo(pthread_t hilo, void* funcion_gestor);
void crear_hilo_solicitudes_kernel();
void* gestionar_solicitudes_kernel();
void crear_hilo_solicitudes_cpu();
void* gestionar_solicitudes_cpu();
void terminar_programa();

#endif