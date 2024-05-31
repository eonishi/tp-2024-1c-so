#ifndef PLANIFICADOR_CORTO_H_
#define PLANIFICADOR_CORTO_H_

#include <semaphore.h>
#include<pthread.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <stdlib.h>
#include "configuracion.h"
#include "colas_planificador.h"
#include "conexion.h"
#include "gestor_io.h"
#include "../../shared/include/pcb.h"
#include "../../shared/include/protocolo.h"


extern t_log *logger;
extern kernel_config* config;
extern int socket_cpu_dispatch, socket_cpu_interrupt, socket_memoria, socket_io;
extern t_list *lista_conexiones_io;

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
void* serializar_interrupcion(unsigned int , size_t*);
void enviar_interrupcion(int , unsigned);


#endif