#ifndef LOGGER_H_
#define LOGGER_H_

#include <commons/log.h>
#include <stdio.h>
#include <stdlib.h>

t_log* iniciar_logger(char* nombre_archivo, char* nombre_proceso);

#endif