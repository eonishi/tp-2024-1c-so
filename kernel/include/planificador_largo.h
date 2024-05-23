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

extern t_queue *cola_new;
extern t_queue *cola_ready;
extern t_queue *cola_exit;
extern int planificacion_activada;
// Semaforos
extern sem_t sem_nuevo_proceso;
extern sem_t sem_grado_multiprog;
extern sem_t sem_proceso_en_ready;

void inicializar_cola_new();
void insertar_en_cola_new(pcb* pcb);
void *iniciar_planificacion_largo();

#endif