#ifndef INTERRUPCION_H_
#define INTERRUPCION_H_

#include <commons/collections/queue.h>
#include "../../shared/include/pcb.h"

extern int socket_cpu_interrupt;
extern t_queue *cola_execute;

void send_interrupt(unsigned PID, int socket_interrupt);
void interrumpir_proceso_ejecutando();

#endif