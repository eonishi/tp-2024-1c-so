#ifndef PLANIFICADOR_LARGO_H_
#define PLANIFICADOR_LARGO_H_

#include <semaphore.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <stdlib.h>
#include "configuracion.h"
#include "../../shared/include/pcb.h"

extern t_log *logger;
extern kernel_config* config;
extern int socket_cpu, socket_memoria;

extern t_queue *colaNew;
extern t_queue *colaReady;
extern int planificador_largo_activado;
// Semaforos
extern sem_t sem_nuevo_proceso;

void inicializar_cola_new();
void inicializar_cola_ready();
void insertar_en_cola_new(pcb* pcb);
void *iniciar_planificacion_largo();

void dispatch_proceso_planificador(pcb* newPcb);



#endif