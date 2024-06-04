#ifndef COLAS_PLANIFICADOR_H_
#define COLAS_PLANIFICADOR_H_

#include <commons/log.h>
#include <commons/collections/queue.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../shared/include/pcb.h"

extern t_log *logger;

extern t_queue *cola_new;
extern t_queue *cola_ready;
extern t_queue *cola_blocked;
extern t_queue *cola_exit;
extern t_queue *cola_execute;

void inicializar_colas_planificador();

void push_cola_new(pcb* pcb);
pcb* pop_cola_new();

void push_cola_ready(pcb* pcb);
pcb* pop_cola_ready();

void push_cola_blocked(pcb* pcb);
pcb* pop_cola_blocked();

void push_cola_exit(pcb* pcb);

void imprimir_colas();
void imprimir_cola(char* nombre, t_queue* cola);

void push_cola_execute(pcb* pcb);
pcb* pop_cola_execute();

#endif