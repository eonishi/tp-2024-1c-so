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

#include "../../shared/include/logger.h"
#include "../../shared/include/client.h"
#include "../../shared/include/server.h"
#include "../../shared/include/comunicacion.h"
#include "../../shared/include/pcb.h"
#include "../../shared/include/protocolo.h"

t_log *logger;
kernel_config* config;
int pcb_counter = 1;
int socket_cpu_dispatch, socket_cpu_interrupt, socket_memoria;
pthread_t hilo_servidor_kernel;
pthread_t hilo_escucha_cpu;

// Variables Planificador
int planificacion_activada = 0;
// -- Colas
t_queue *cola_new;
t_queue *cola_exit;
t_queue *cola_ready;
t_queue *cola_blocked;
// -- Semaforos
sem_t sem_nuevo_proceso;
sem_t sem_grado_multiprog;
sem_t sem_proceso_en_ready;
sem_t sem_cpu_libre;
// -- Hilos
pthread_t hilo_planificador_largo;
pthread_t hilo_planificador_corto;
pthread_t hilo_planificador_corto_RR;
pthread_t hilo_quantum;
// Fin variables planificador


t_list* procesoNew;
t_list* procesoReady;
t_list* procesoExecute;
t_list* procesoBlock;
t_list* procesoExit;

sem_t bloque; //semaforo para activar y desactivar en un mismo bloque de codigo

typedef enum
{
	CPU,
	MEMORIA,
} cod_mensaje;

void terminar_programa();
void iniciar_consola();
void *iniciar_escucha_servidor();
void iniciar_servidor_en_hilo();
pcb* iniciar_proceso_en_memoria(char* filePath);
void dispatch_proceso();
void iniciar_hilo(void* func, pthread_t thread);






#endif