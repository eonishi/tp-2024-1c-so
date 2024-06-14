#ifndef EXECUTE_H_
#define EXECUTE_H_

#include <commons/log.h>
#include <commons/string.h>
#include "registro.h"
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
void exec_io_stdin_read(char** instr_tokenizada);

// MOV_OUT
int existe_en_tlb(uint32_t direccion_fisica);
uint32_t obtener_direccion_fisica_de_tlb(uint32_t* direccion_logica);
uint32_t calcular_direccion_fisica(uint32_t* direccion_logica);

#endif