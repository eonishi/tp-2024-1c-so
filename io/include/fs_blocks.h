#ifndef IO_FS_BLOCKS_H_
#define IO_FS_BLOCKS_H_

#include <commons/config.h>
#include <commons/log.h>
#include <stdlib.h>
#include <string.h>
#include "fs_fcb.h"
#include "fs_utils.h"
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

bool escribir_datos_en_bloques(int posicion_inicial, void* datos, int tam_total);
bool leer_datos_en_bloques(int posicion_inicial, int tam_total, void** datos_leidos);
int buscar_bloque_libre();
int asignar_bloque();
void asignar_bloques_bitmap_por_rango(int desde, int hasta);
void eliminar_bloques_ocupados_por_archivo(char* nombre);

bool esta_bloque_ocupado(int index);
int calcular_bloques_necesarios(int size);

void liberar_bitmap_de_bloques();
void liberar_bloques_bitmap_por_rango(int desde, int hasta);

void reubicar_archivo_desde_fcb(fcb* fcb);

int calcular_byte_inicial_de_bloque(int bloque_inicial);

//
bool archivo_esta_vacio(int fd);
void *enlazar_archivo(int fd, int tam_archivo);
void compactar();




#endif