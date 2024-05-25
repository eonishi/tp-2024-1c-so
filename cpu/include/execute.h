#ifndef EXECUTE_H_
#define EXECUTE_H_

#include <commons/log.h>
#include <commons/string.h>
#include "operacion.h"
#include "../../shared/include/pcb.h"
#include "../../shared/include/protocolo.h"

extern t_log* logger;
extern pcb* pcb_actual;
extern int tengo_pcb;
extern int socket_kernel;

void execute(char** instruccion_tokenizada);
void exec_set(char**);
void exec_sum(char**);
void exec_sub(char**);
void exec_jnz(char**);
void exec_io_gen_sleep(char** instr_tokenizada);

#endif