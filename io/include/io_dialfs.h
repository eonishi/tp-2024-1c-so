#ifndef IO_DIALFS_H_
#define IO_DIALFS_H_

#include "configuracion.h"

extern t_log *logger;
extern io_config config;
extern int kernel_socket;

void io_dialfs();

#endif