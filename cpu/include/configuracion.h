#ifndef CONFIGURACION_CPU_H_
#define CONFIGURACION_CPU_H_

#include <commons/log.h>
#include <commons/config.h>

extern t_log *logger;

typedef struct
{
    char *ip_cpu;
    char *puerto_dispatch;
    char *puerto_interrupt;
    char *ip_memoria;
    char *puerto_memoria;
    unsigned cantidad_entradas_tlb;
    char *algoritmo_tlb;
} cpu_config;

void inicializar_configuracion();

#endif