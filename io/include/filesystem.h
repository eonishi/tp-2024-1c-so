#ifndef IO_FILESYSTEM_H_
#define IO_FILESYSTEM_H_

#include <commons/config.h>
#include <string.h>
#include <stdlib.h>
#include "configuracion.h"
#include "fs_fcb.h"
#include <commons/bitarray.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

extern t_log *logger;
extern io_config config;
extern int kernel_socket;

// Init
bool inicializar_filesystem();
void inicializar_bloques();
void inicializar_bitmap();
bool inicializar_bloques_en_archivo(int fd);
bool inicializar_bitmap_en_archivo(int fd);

bool archivo_esta_vacio(int fd);
void *enlazar_archivo(int fd, int tam_archivo);

bool crear_archivo(char* nombre);
bool truncar_archivo(char* nombre, int size);
void guardar_archivo_desde_fcb(fcb* fcb);


// Bloques
int asignar_bloque();
int buscar_bloque_libre();
int calcular_bloques_a_ocupar(int size);
bool hay_bloques_contiguos_para_extender(int bloque_inicial, int cantidad_bloques_actuales, int bloques_necesarios);
bool hay_bloques_libres_suficientes(int bloques_necesarios);
void asignar_bloques_bitmap_por_rango(int desde, int hasta);
void liberar_bloques_bitmap_por_rango(int desde, int hasta);

// Utiles
void imprimir_bitmap();

#endif