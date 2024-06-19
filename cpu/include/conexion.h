#ifndef CONEXION_CPU_H_
#define CONEXION_CPU_H_

#include "configuracion.h"
#include "ciclo.h"
#include <commons/log.h>
#include "../../shared/include/server.h"
#include "../../shared/include/client.h"
#include "../../shared/include/comunicacion.h"
#include "../../shared/include/pcb.h"

extern t_log *logger;
extern cpu_config config;
extern pcb* pcb_actual;
extern int tengo_pcb;
extern unsigned TAM_PAGINA;
extern t_list tlb;

int crear_conexion_memoria();
void esperar_handshake_kernel(int server);
void server_dispatch();
void server_interrupt();

#endif