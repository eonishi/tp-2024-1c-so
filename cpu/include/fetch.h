#ifndef FETCH_H_
#define FETCH_H_

#include "../../shared/include/comunicacion.h"
#include "../../shared/include/protocolo.h"
#include "../../shared/include/logger.h"
#include <commons/string.h>

extern t_log* logger;
extern int socket_memoria;
extern pcb* pcb_actual;

char* fetch(); // Retorna la instruccion que recibe de memoria
char* recibir_instruccion_de_memoria(int socket_memoria);
void pedir_intruccion_a_memoria(unsigned PC);


#endif