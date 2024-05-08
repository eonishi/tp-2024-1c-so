#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "configuracion.h"
#include "conexion.h"
#include "../../shared/include/logger.h"

t_log* logger;
extern memory_config config;

extern int socket_cpu, socket_kernel;

pthread_t hilo_cpu;
pthread_t hilo_kernel;

void iniciar_gestor_solicitudes_en_hilo(pthread_t hilo, void* funcion_gestor);
void crear_hilo_solicitudes_kernel();
void* gestionar_solicitudes_kernel();
void crear_hilo_solicitudes_cpu();
void* gestionar_solicitudes_cpu();

void terminar_programa();

#endif