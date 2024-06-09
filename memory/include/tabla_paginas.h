#ifndef TABLAS_DE_PAGINAS_H_
#define TABLAS_DE_PAGINAS_H_

#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include "proceso_mem.h"
#include "frame.h"

typedef t_dictionary t_tabla_paginas;

extern t_list* procesos_en_memoria;

t_dictionary *crear_tabla_paginas();
unsigned calcular_cantidad_de_paginas_por_bytes(unsigned bytes);

#endif