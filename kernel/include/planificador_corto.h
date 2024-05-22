#ifndef PLANIFICADOR_CORTO_H_
#define PLANIFICADOR_CORTO_H_

#include <semaphore.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <stdlib.h>
#include "configuracion.h"
#include "../../shared/include/pcb.h"

extern t_log *logger;
extern kernel_config* config;
extern int socket_cpu, socket_memoria;

extern t_queue *colaReady;
extern int planificacion_activada;
// Semaforos
extern sem_t sem_proceso_en_ready;


void inicializar_cola_ready();
void *iniciar_planificacion_corto();
void dispatch_proceso_planificador(pcb* newPcb);

#endif