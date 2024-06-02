#ifndef OPERACION_H_
#define OPERACION_H_

#include <stdlib.h>
#include <commons/string.h>
#include <commons/log.h>
#include "io_tipos.h"

extern t_log* logger;

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
    EXIT_OP
}operacion;

typedef struct {
    char* key;
    operacion value;
} table_element; // pseudo diccionario estático

operacion get_operacion(char** instr_tokenizada);
int get_valor(char* token_valor);
bool io_tiene_operacion(int *operaciones_io, char *operacion_token, io_tipo tipo_io);
bool operacion_usa_memoria(char *token_operacion);

#endif