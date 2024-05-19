#ifndef CICLO_H_
#define CICLO_H_

#include "../../shared/include/protocolo.h"
#include "../../shared/include/pcb.h"

extern int socket_dispatch;
extern int INTERRUPTION_FLAG, tengo_pcb;
extern pcb *pcb_actual;

void checkInterrupt();

#endif
