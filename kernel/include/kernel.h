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

#include "../../shared/include/logger.h"
#include "../../shared/include/client.h"
#include "../../shared/include/server.h"
#include "../../shared/include/comunicacion.h"
#include "../../shared/include/pcb.h"
#include "../../shared/include/protocolo.h"

t_log *logger;
kernel_config* config;
int socket_cpu_dispatch, socket_cpu_interrupt, socket_memoria, socket_io, socket_server_kernel;
t_list *lista_conexiones_io;

pthread_t hilo_servidor_kernel;
pthread_t hilo_escucha_cpu;

// Variables Planificador
int planificacion_activada = 0;

// -- Colas
extern t_queue *cola_new, *cola_exit, *cola_ready, *cola_blocked, *cola_execute, *cola_readyVRR;
// -- Semaforos
sem_t sem_nuevo_proceso;
sem_t sem_grado_multiprog;
sem_t sem_proceso_en_ready;
sem_t sem_cpu_libre;
// -- Hilos
pthread_t hilo_planificador_largo;
pthread_t hilo_conexiones_io;
pthread_t hilo_conexiones_io2;
pthread_t hilo_planificador_corto;
pthread_t hilo_planificador_corto_RR;
pthread_t hilo_planificador_corto_VRR;

// Fin variables planificador

typedef enum
{
	CPU,
	MEMORIA,
} cod_mensaje;


void terminar_programa();
void iniciar_semaforos();
void iniciar_servidor_en_hilo();
void dispatch_proceso();
void iniciar_hilo(void *func, pthread_t thread);
void *esperar_y_escuchar_conexiones_io();
void iniciar_hilo_con_args(void *(*func)(void *), pthread_t thread, void *args);
//void crear_hilo_planificador_corto();
//void cancelar_hilo_planificador();
//void cambiar_algoritmo_planificadorCorto(algoritmo_planificador);

#endif