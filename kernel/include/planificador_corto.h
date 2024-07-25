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
#include "interrupcion.h"
#include "recurso.h"
#include "detener_planificacion.h"
#include "../../shared/include/pcb.h"
#include "../../shared/include/protocolo.h"
#include "../../shared/include/direccion.h"
#include "commons/temporal.h"
#include "../../shared/include/codigos_operacion.h"

extern pthread_t hilo_quantum;

extern t_log *logger;
extern t_log *aux_log;
extern kernel_config* config;
extern int socket_cpu_dispatch, socket_cpu_interrupt, socket_memoria, socket_io, q_usado, q_restante;
extern t_list *lista_conexiones_io;
extern t_temporal *q_transcurrido;
extern t_queue *cola_readyVRR;

extern int planificacion_activada;
// Semaforos
extern sem_t sem_proceso_en_ready;
extern sem_t sem_grado_multiprog;
extern sem_t sem_cpu_libre;

void *iniciar_planificacion_corto();
void *iniciar_planificacion_corto_RR();
void *iniciar_planificacion_corto_VRR();
void dispatch_proceso_planificador(pcb* newPcb);
void gestionar_respuesta_cpu();
void *monitoreo_quantum();
void crear_hilo_quantum();
void cancelar_hilo_quantum();
const char* traduce_cod_op(op_code code); //lo uso para cumplir con los logs minimos


#endif