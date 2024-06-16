#ifndef CHECKINTERRUPT_H_
#define CHECKINTERRUPT_H_

#include "../../shared/include/codigos_operacion.h"
#include "../../shared/include/pcb.h"



extern int socket_kernel;
extern int INTERRUPTION_FLAG, tengo_pcb;
extern pcb *pcb_actual;

int checkInterrupt();

#endif
