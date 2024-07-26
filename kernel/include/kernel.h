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
#include <semaphore.h>

#include "configuracion.h"
#include "conexion.h"
#include "planificador_largo.h"
#include "planificador_corto.h"
#include "gestor_io.h"
#include "consola.h"
#include "recurso.h"

#include "../../shared/include/logger.h"
#include "../../shared/include/client.h"
#include "../../shared/include/server.h"
#include "../../shared/include/comunicacion.h"
#include "../../shared/include/pcb.h"
#include "../../shared/include/protocolo.h"
#include "../../shared/include/hilo.h"

t_log *logger;
t_log *logger_oblig;
kernel_config* config;
int socket_cpu_dispatch, socket_cpu_interrupt, socket_memoria, socket_io, socket_server_kernel;
extern t_list *lista_conexiones_io;
extern pthread_mutex_t mutex_conexiones_io;

// -- Semaforos
sem_t sem_nuevo_proceso;
sem_t sem_grado_multiprog;
sem_t sem_proceso_en_ready;
sem_t sem_cpu_libre;
extern sem_t sem_planificacion_activa;

void terminar_programa();
void iniciar_semaforos();
void iniciar_servidor_en_hilo();
void dispatch_proceso();
void *esperar_y_escuchar_conexiones_io();
void crear_hilo_planificador_corto();
void cancelar_hilo_planificador();

#endif