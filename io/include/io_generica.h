#ifndef IO_GENERICA_H_
#define IO_GENERICA_H_

#include "conexion.h"
#include "configuracion.h"
#include <stdlib.h>
#include <unistd.h>

extern t_log *logger;
extern t_log *logger_oblig;
extern io_config config;
extern int kernel_socket;

void io_generica();

#endif
