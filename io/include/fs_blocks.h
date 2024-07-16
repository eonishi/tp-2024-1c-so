#ifndef IO_FS_BLOCKS_H_
#define IO_FS_BLOCKS_H_

#include <commons/config.h>
#include <commons/log.h>
#include <stdlib.h>
#include <string.h>
#include "fs_fcb.h"
#include <fcntl.h>
#include <sys/stat.h>
#include "configuracion.h"
#include <sys/mman.h> // requerido para PROT_READ | PROT_WRITE, MAP_SHARED

extern t_log *logger;
extern io_config config;

// Init
void inicializar_bloques();
void inicializar_archivo_bloques();
void inicializar_archivo_bitmap();
bool inicializar_bloques_en_archivo(int fd);
bool inicializar_bitmap_en_archivo(int fd);

// Utils
void imprimir_bitmap();
int asignar_bloque();
int buscar_bloque_libre();

//
bool archivo_esta_vacio(int fd);
void *enlazar_archivo(int fd, int tam_archivo);
void asignar_bloques_bitmap_por_rango(int desde, int hasta);
void liberar_bitmap_de_bloques();
void liberar_bloques_bitmap_por_rango(int desde, int hasta);
bool esta_bloque_ocupado(int index);
int calcular_bloques_a_ocupar(int size);



#endif