#ifndef SERIALIZAR_H_
#define SERIALIZAR_H_

#include <stdint.h>
#include <stddef.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include "io_tipos.h"
#include "paquete.h"

// Temporales
void* serializar_char(char* string);
char* deserializar_char(void* char_bytes, int8_t size);

void* serializar_int(int number);
int deserializar_int(void *int_bytes);

void* serializar_uint32(uint32_t value);
uint32_t deserializar_uint32(void* int_bytes);

void* serializar_int8(int8_t value);
int8_t deserializar_int8(void* int_bytes);

void* serializar_lista_strings(char** strings);
char** deserializar_lista_strings(t_list* bytes, int index_cantidad_tokens);
void serializar_lista_strings_y_agregar_a_paquete(char **lista_strings, t_paquete *paquete);
void liberar_lista_strings(char **lista_strings);

#endif