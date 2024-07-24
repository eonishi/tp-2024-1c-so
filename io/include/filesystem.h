#ifndef IO_FILESYSTEM_H_
#define IO_FILESYSTEM_H_

#include <commons/config.h>
#include <string.h>
#include <stdlib.h>
#include "configuracion.h"
#include "fs_fcb.h"
#include "fs_blocks.h"
#include "fs_utils.h"
#include <commons/bitarray.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

extern t_log *logger;
extern io_config config;
extern int kernel_socket;

bool inicializar_filesystem();
bool crear_archivo(char *nombre);
bool eliminar_archivo(char* nombre);
bool escribir_archivo(char*nombre, void* datos, int tam_total, int puntero_archivo);
bool leer_archivo(char*nombre, int puntero_archivo, int tam_a_leer, void** datos_leidos);
bool truncar_archivo(char* nombre, int size);
void guardar_archivo_desde_fcb(fcb* fcb);
void compactar();

// Bloques
bool hay_bloques_contiguos_para_extender(int bloque_inicial, int cantidad_bloques_actuales, int bloques_necesarios);
bool hay_bloques_libres_suficientes(int bloques_necesarios);

// Utiles
void imprimir_bitmap();

#endif