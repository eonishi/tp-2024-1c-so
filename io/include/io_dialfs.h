#ifndef IO_DIALFS_H_
#define IO_DIALFS_H_

#include "configuracion.h"
#include "conexion.h"
#include <commons/bitarray.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "filesystem.h"

extern t_log *logger;
extern t_log *logger_oblig;
extern io_config config;
extern int kernel_socket;
extern int memory_socket;

void io_dialfs();
static void controlar_peticion();

#endif