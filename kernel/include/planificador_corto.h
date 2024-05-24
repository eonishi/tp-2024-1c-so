#ifndef PLANIFICADOR_CORTO_H_
#define PLANIFICADOR_CORTO_H_

#include <semaphore.h>
#include<pthread.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <stdlib.h>
#include "configuracion.h"
#include "colas_planificador.h"
#include "../../shared/include/pcb.h"


extern t_log *logger;
extern kernel_config* config;
extern int socket_cpu, socket_memoria;

extern int planificacion_activada;
// Semaforos
extern sem_t sem_proceso_en_ready;
extern sem_t sem_grado_multiprog;
extern sem_t sem_cpu_libre;

void *iniciar_planificacion_corto();
void *iniciar_planificacion_corto_RR();
void dispatch_proceso_planificador(pcb* newPcb);
void send_interrupt();
void gestionar_respuesta_cpu();
void *monitoreo_quantum();


#endif