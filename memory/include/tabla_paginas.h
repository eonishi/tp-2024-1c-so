#ifndef TABLAS_DE_PAGINAS_H_
#define TABLAS_DE_PAGINAS_H_

#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include "proceso_mem.h"
#include "frame.h"

typedef t_list t_tabla_paginas;

extern t_list* procesos_en_memoria;

t_tabla_paginas *crear_tabla_paginas();
unsigned get_frame_number_by_pagina(unsigned nro_pagina);
void redimensionar_memoria_proceso(size_t cantidad_paginas);
void mostrar_tabla_paginas();
bool puedo_agregar_o_disminuir(int cantidad_paginas);

#endif