#ifndef FETCH_H_
#define FETCH_H_

#include <commons/log.h>
#include <commons/string.h>
#include "../../shared/include/pcb.h"

extern t_log* logger;
extern pcb* pcb_actual;

void* get_registro(char* token_registro);
bool es8int(char* token_registro);
unsigned tam_registro(char* token_registro);
uint32_t reg_to_uint32(void* valor, char* token);

#endif