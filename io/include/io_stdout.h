#ifndef IO_STDOUT_H_
#define IO_STDOUT_H_

#include "configuracion.h"
#include "../../shared/include/comunicacion.h"
#include "../../shared/include/direccion.h"

extern t_log *logger;
extern t_log *logger_oblig;
extern io_config config;
extern int kernel_socket, memory_socket;

void io_stdout();

#endif