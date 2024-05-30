#ifndef IO_STDOUT_H_
#define IO_STDOUT_H_

#include "configuracion.h"

extern t_log *logger;
extern io_config config;
extern int kernel_socket;

void io_stdout();

#endif