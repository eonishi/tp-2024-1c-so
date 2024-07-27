#ifndef MMU_H_
#define MMU_H_

#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "tlb.h"
#include "../../shared/include/comunicacion.h"
#include "../../shared/include/direccion.h"
#include "../../shared/include/pcb.h"
#include "../../shared/include/protocolo.h"

extern t_log* logger_oblig;
extern int socket_memoria;
extern pcb *pcb_actual;

t_list* mmu(uint32_t direccion_logica, size_t tam_dato, void* dato);

#endif