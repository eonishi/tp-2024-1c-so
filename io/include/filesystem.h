#ifndef IO_FILESYSTEM_H_
#define IO_FILESYSTEM_H_

#include <commons/config.h>
#include <string.h>
#include <stdlib.h>
#include "configuracion.h"
#include <commons/bitarray.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

extern t_log *logger;
extern io_config config;
extern int kernel_socket;

typedef struct 
{
    char* nombre;
    t_config* config;
} fcb;

bool inicializar_filesystem();
void inicializar_bloques();
void inicializar_bitmap();

bool inicializar_bloques_en_archivo(int fd);
bool inicializar_bitmap_en_archivo(int fd);

bool archivo_esta_vacio(int fd);
void *enlazar_archivo(int fd, int tam_archivo);

bool crear_archivo(char* nombre);
int asignar_bloque();
int buscar_bloque_libre();
int calcular_bloques_a_ocupar(int size);
void guardar_archivo_desde_fcb(fcb* fcb);

void imprimir_bitmap();

void set_campo_de_archivo(char* campo , int valor, t_config* config_loader);

bool truncar_archivo(char* nombre, int size);

bool hay_bloques_contiguos_para_extender(int bloque_inicial, int cantidad_bloques_actuales, int bloques_necesarios);
bool hay_bloques_libres_suficientes(int bloques_necesarios);

void asignar_bloques_bitmap_por_rango(int desde, int hasta);
void liberar_bloques_bitmap_por_rango(int desde, int hasta);

// Utils fcb_list
fcb* obtener_fcb_por_nombre(char* nombre);
void eliminar_fcb_por_nombre(char* nombre);
bool condicion_por_nombre(void* file_control_block);

#endif