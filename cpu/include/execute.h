#ifndef EXECUTE_H_
#define EXECUTE_H_

#include <commons/log.h>
#include <commons/string.h>
#include "registro.h"
#include "mmu.h"
#include "../../shared/include/operacion.h"
#include "../../shared/include/pcb.h"
#include "../../shared/include/protocolo.h"

extern t_log* logger;
extern pcb* pcb_actual;
extern int tengo_pcb;
extern int socket_kernel, socket_memoria;
extern unsigned TAM_PAGINA;

void execute(char** instruccion_tokenizada);
void exec_set(char**);
void exec_sum(char**);
void exec_sub(char**);
void exec_jnz(char**);
void exec_operacion_io(char** instr_tokenizada);
void exec_resize(char** instr_tokenizada);

void exec_mov_in(char** instr_tokenizada);
void exec_mov_out(char** instr_tokenizada);
void exec_cp_string(char **instr_tokenizada);
void exec_io_stdout_write(char **instr_tokenizada);
void exec_io_stdin_read(char** instr_tokenizada);

void desalojar_pcb(char **instr_tokenizada, op_code codigo);

void exec_io_fs_read_write(char** instr_tokenizada);
void reemplazar_registro_por_dato(char** instr_tokenizada, int index, int valor);

#endif