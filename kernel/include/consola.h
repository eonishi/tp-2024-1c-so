#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <stdio.h>
#include <semaphore.h>
#include <readline/readline.h>
#include <commons/string.h>
#include "configuracion.h"
#include "colas_planificador.h"
#include "../../shared/include/table_element.h"
#include "../../shared/include/pcb.h"
#include "../../shared/include/protocolo.h"

extern kernel_config* config;
extern sem_t sem_nuevo_proceso;
extern int planificacion_activada;
extern int socket_memoria;

typedef enum {
	EJECUTAR_SCRIPT,
	INICIAR_PROCESO,
	FINALIZAR_PROCESO,
	INICIAR_PLANIFICACION,
	DETENER_PLANIFICACION,
	MULTIPROGRAMACION,
	PROCESO_ESTADO
} comando_consola;

void iniciar_consola();

#endif