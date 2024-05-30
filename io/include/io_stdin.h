#ifndef IO_STDIN_H_
#define IO_STDIN_H_

#include "configuracion.h"

extern t_log *logger;
extern io_config config;
extern int kernel_socket;

void io_stdin();

#endif