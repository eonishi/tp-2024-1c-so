#ifndef FRAME_H_
#define FRAME_H_

#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/bitarray.h>
#include <string.h>
#include "configuracion.h"

extern memory_config config;

void inicializar_memoria();
void* get_frame(int frame_number, int offset);
void set_frame(int frame_number, int offset, void *data, size_t size_data);
bool tengo_espacio_para_agregar(int cantidad_frames);
void imprimir_frames();
void imprimir_data(int frame_number, int offset, size_t value_size);

#endif
