#ifndef TABLAS_DE_PAGINAS_H_
#define TABLAS_DE_PAGINAS_H_

#include <pthread.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include "proceso_mem.h"
#include "frame.h"

typedef t_list t_tabla_paginas;
struct t_proceso_en_memoria;

extern t_list* procesos_en_memoria;
extern unsigned PID_a_liberar, PID_solicitado;
extern pthread_mutex_t mutex_procesos_en_memoria;

t_tabla_paginas *crear_tabla_paginas();
unsigned get_frame_number_by_pagina(unsigned nro_pagina);
void redimensionar_memoria_proceso(size_t cantidad_paginas);
void mostrar_tabla_paginas();
bool puedo_agregar_o_disminuir(int cantidad_paginas);
unsigned calcular_cantidad_de_paginas_por_bytes(unsigned bytes);
void quitar_paginas(size_t cantidad_de_paginas, t_proceso_en_memoria *proceso);
unsigned obtener_tamanio_actual_proceso();
size_t cantidad_de_paginas();

#endif