#ifndef PLANIFICADOR_LARGO_H_
#define PLANIFICADOR_LARGO_H_

#include <semaphore.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <stdlib.h>
#include "configuracion.h"
#include "colas_planificador.h"
#include "detener_planificacion.h"
#include "../../shared/include/pcb.h"

extern t_log *logger;
extern t_log *logger_oblig;
extern kernel_config* config;
extern int socket_cpu, socket_memoria;

extern int planificacion_activada;
// Semaforos
extern sem_t sem_nuevo_proceso;
extern sem_t sem_grado_multiprog;
extern sem_t sem_proceso_en_ready;

void *iniciar_planificacion_largo();

#endif