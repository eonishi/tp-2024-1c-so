#ifndef FETCH_H_
#define FETCH_H_

#include "../../shared/include/comunicacion.h"
#include "../../shared/include/paquete.h"
#include "../../shared/include/protocolo.h"
#include "../../shared/include/pcb.h"
#include <commons/log.h>
#include <commons/string.h>

extern t_log* logger;
extern t_log* logger_oblig;
extern int socket_memoria;
extern pcb* pcb_actual;

char* fetch(); // Retorna la instruccion que recibe de memoria
char* recibir_instruccion_de_memoria(int socket_memoria);
void pedir_intruccion_a_memoria();
void *serializar_u(void *datos, size_t size);

#endif