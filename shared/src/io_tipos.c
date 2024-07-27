#include "../include/io_tipos.h"

size_t size_io_op(io_tipo tipo){
    return (tipo == DIALFS ? sizeof(int)*5 : sizeof(int));
}

void* serializar_io_operaciones(int* instrucciones, size_t size){
    void* stream = malloc(size);
    memcpy(stream, instrucciones, size);
    return stream;
}
int* deserializar_io_operaciones(void* instr_bytes, io_tipo tipo){
    size_t size = size_io_op(tipo);
    int* instrucciones = malloc(size);
    memcpy(instrucciones, instr_bytes, size);
    return instrucciones;
}