#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <stdio.h>
#include <semaphore.h>
#include <readline/readline.h>
#include <commons/string.h>
#include "configuracion.h"
#include "colas_planificador.h"
#include "interrupcion.h"
#include "planificador_corto.h"
//#include "kernel.h"
#include "../../shared/include/table_element.h"
#include "../../shared/include/pcb.h"
#include "../../shared/include/protocolo.h"

extern kernel_config* config;
extern sem_t sem_nuevo_proceso;
extern int planificacion_activada;
extern int socket_memoria;

extern pthread_t hilo_planificador_corto;
extern pthread_t hilo_planificador_corto_RR;
extern pthread_t hilo_planificador_corto_VRR;

typedef enum {
	EJECUTAR_SCRIPT,
	INICIAR_PROCESO,
	FINALIZAR_PROCESO,
	INICIAR_PLANIFICACION,
	DETENER_PLANIFICACION,
	MULTIPROGRAMACION,
	PROCESO_ESTADO,
	INICIAR_PLANIFICADOR
} comando_consola;

void iniciar_consola();
void crear_hilo_planificador_corto();
void cancelar_hilo_planificador();
#endif