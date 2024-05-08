#ifndef OPERACION_H_
#define OPERACION_H_

#include <commons/string.h>
#include <commons/log.h>
#include "../../shared/include/pcb.h"

extern t_log* logger;
extern pcb* pcb_actual;

typedef enum {
    SET,
    MOV_IN,
    MOV_OUT,
    SUM,
    SUB,
    JNZ,
    RESIZE,
    COPY_STRING,
    WAIT,
    SIGNAL,
    IO_GEN_SLEEP,
    IO_STDIN_READ,
    IO_STDOUT_WRITE,
    IO_FS_CREATE,
    IO_FS_DELETE,
    IO_FS_TRUNCATE,
    IO_FS_WRITE,
    IO_FS_READ,
    EXIT
}operacion;

typedef struct {
    const char* key;
    operacion value;
} table_element; // pseudo diccionario estático

operacion get_operacion(char** instr_tokenizada);
void* get_registro(char* token_registro);
int get_valor(char* token_valor);

#endif