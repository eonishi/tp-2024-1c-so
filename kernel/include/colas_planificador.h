#ifndef COLAS_PLANIFICADOR_H_
#define COLAS_PLANIFICADOR_H_

#include <commons/log.h>
#include <commons/collections/queue.h>
#include <stdlib.h>
#include <stdio.h>
#include "conexion.h"
#include "../../shared/include/pcb.h"

extern t_log *logger;
extern int socket_memoria;

typedef struct {
    pcb* pcbVRR;
    int quantum_usado;
} elemVRR;

extern elemVRR *elemento_VRR;
extern t_list *lista_conexiones_io;

void inicializar_colas_planificador();

void push_cola_new(pcb* pcb);
pcb* pop_cola_new();

void push_cola_ready(pcb* pcb);
pcb* pop_cola_ready();

void push_cola_blocked(pcb* pcb, t_queue* cola_blocked);

void push_cola_exit(pcb* pcb);

void imprimir_colas();
void imprimir_cola(char* nombre, t_queue* cola);
void imprimir_colas_io();

void push_cola_execute(pcb* pcb);
pcb* pop_cola_execute();

void push_cola_ready_priority(pcb* pcbVRR, int q_pendiente);
elemVRR* pop_cola_ready_priority();

void pop_and_destroy(t_queue *queue, void (*destroyer)(void *));
void finalizar_proceso(unsigned PID);

#endif