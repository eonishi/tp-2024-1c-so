#ifndef IO_TIPOS_H_
#define IO_TIPOS_H_

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    GENERICA,
    STDIN,
    STDOUT,
    DIALFS
} io_tipo;

size_t size_io_op(io_tipo tipo);
void* serializar_io_operaciones(int* instrucciones, size_t size);
int* deserializar_io_operaciones(void* instr_bytes, io_tipo tipo);

#endif