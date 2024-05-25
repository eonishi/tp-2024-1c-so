#ifndef IO_H_
#define IO_H_

#include "inicializar.h"
#include <commons/log.h>

extern t_log *logger;

void check_args(int argc, char** argv, char*, char*);

#endif