#ifndef TLB_H_
#define TLB_H_

#include <stdlib.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include "configuracion.h"

extern cpu_config config;
extern t_log *logger;

typedef struct{
    unsigned PID;
    unsigned pagina;
    unsigned frame;
    unsigned uso_tiempo;
} tlb_entry;

void iniciar_tlb();
void agregar_entrada_tlb(unsigned pid, unsigned pagina, unsigned frame);
bool tlb_tiene_entrada(unsigned pid, unsigned pagina);
unsigned obtener_frame_tlb(unsigned pid, unsigned pagina);

#endif
