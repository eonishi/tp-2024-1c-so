#ifndef EXECUTE_H_
#define EXECUTE_H_

#include <commons/log.h>
#include <commons/string.h>
#include "operacion.h"
#include "../../shared/include/pcb.h"

extern t_log* logger;
extern pcb* pcb_actual;

void execute(char** instruccion_tokenizada);
void exec_set();
void exec_sum();
void exec_sub();
void exec_jnz();
void exec_io_gen_sleep();

#endif