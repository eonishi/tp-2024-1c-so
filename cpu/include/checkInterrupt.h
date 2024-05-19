#ifndef CICLO_H_
#define CICLO_H_

#include "../../shared/include/codigos_operacion.h"
#include "../../shared/include/pcb.h"

extern int socket_kernel;
extern int INTERRUPTION_FLAG, tengo_pcb;
extern pcb *pcb_actual;

void checkInterrupt();

#endif
