#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include "../../shared/include/logger.h"
#include "../../shared/include/server.h"
#include "../../shared/include/client.h"
#include "../../shared/include/comunicacion.h"
#include "../../shared/include/pcb.h"
#include "ciclo.h"

t_log *logger;
int socket_memoria, socket_kernel;

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

cpu_config config;

void inicializar_configuracion();
void terminar_programa();

int crear_conexion_memoria();
void esperar_handshake_kernel(int server);

// Server threads
void server_dispatch();
void server_interrupt();

#endif