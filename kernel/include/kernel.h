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
#include "../../shared/include/protocolo.h"
#include <semaphore.h>
#include "planificador_largo.h"

t_log *logger;
kernel_config* config;
int pcb_counter = 1;
int socket_cpu, socket_memoria;
pthread_t hilo_servidor_kernel;

// Largo Plazo
pthread_t hilo_planificador_largo;
t_queue *colaNew;
t_queue *colaReady;
int planificador_largo_activado = 1;
// Semaforos
sem_t sem_nuevo_proceso;
//

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
void *iniciar_escucha();
void iniciar_servidor_en_hilo();
pcb* iniciar_proceso_en_memoria(char* filePath);
void dispatch_proceso();






#endif