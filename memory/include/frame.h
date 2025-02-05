#ifndef FRAME_H_
#define FRAME_H_

#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/bitarray.h>
#include <commons/memory.h>
#include <string.h>
#include "configuracion.h"

extern memory_config config;

void inicializar_memoria();
void set_memoria(uint32_t direccion_fisica, void *data, size_t size_data);
void *get_memoria(uint32_t direccion_fisica);
bool tengo_espacio_para_agregar(int cantidad_frames);
void marcar_frame_como(unsigned frame_number, int estado);
unsigned get_available_frame();
unsigned frames_libres();
void imprimir_memoria_hex();

#endif
