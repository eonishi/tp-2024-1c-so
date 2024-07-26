#ifndef COLAS_PLANIFICADOR_H_
#define COLAS_PLANIFICADOR_H_

#include <commons/log.h>
#include <commons/collections/queue.h>
#include <stdlib.h>
#include <stdio.h>
#include "conexion.h"
#include "recurso.h"
#include "interrupcion.h"
#include "../../shared/include/pcb.h"

extern t_log *logger;
extern t_log *aux_log;
extern int socket_memoria;
extern t_list *recursos_disponibles;

typedef struct {
    pcb* pcbVRR;
    int quantum_usado;
} elemVRR;

extern elemVRR *elemento_VRR;
extern t_list *lista_conexiones_io;
extern sem_t sem_proceso_en_ready, sem_grado_multiprog, sem_nuevo_proceso;
extern int diferencia_grado_multiprog;

void inicializar_colas_planificador();

void push_cola_new(pcb* pcb);
pcb* pop_cola_new();

void push_cola_ready(pcb* pcb);
pcb* pop_cola_ready();

void push_cola_blocked(pcb* pcb, t_queue* cola_blocked, sem_t* sem_blocked, pthread_mutex_t mutex_blocked);

void push_cola_exit(pcb* pcb);

void imprimir_colas();
void imprimir_cola(char* nombre, t_queue* cola, pthread_mutex_t mutex);
void imprimir_colas_io();
void imprimir_colas_recurso();

void push_cola_execute(pcb* pcb);
pcb* pop_cola_execute();
void pop_and_destroy_execute();

void push_cola_ready_priority(pcb* pcbVRR, int q_pendiente);
elemVRR* pop_cola_ready_priority();

void pop_and_destroy(t_queue *queue, void (*destroyer)(void *), pthread_mutex_t mutex);
void finalizar_proceso(unsigned PID);

#endif