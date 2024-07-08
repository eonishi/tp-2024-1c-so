#ifndef IO_STDIN_H_
#define IO_STDIN_H_

#include "conexion.h"
#include "configuracion.h"
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>


extern t_log *logger;
extern io_config config;
extern int kernel_socket, memory_socket;

void io_stdin();
char* iniciar_consola(uint32_t tamanio_maximo);

#endif