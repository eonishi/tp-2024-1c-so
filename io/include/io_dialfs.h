#ifndef IO_DIALFS_H_
#define IO_DIALFS_H_

#include "configuracion.h"
#include <commons/bitarray.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

extern t_log *logger;
extern io_config config;
extern int kernel_socket;

void io_dialfs();

bool archivo_esta_vacio(FILE* fd);
void *enlazar_archivo(FILE *fd);
bool inicializar_bloques_en_archivo(FILE* fd);

#endif