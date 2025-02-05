#ifndef INICIALIZAR_IO_H_
#define INICIALIZAR_IO_H_

#include <commons/string.h> 
#include "io_generica.h"
#include "io_stdin.h"
#include "io_stdout.h"
#include "io_dialfs.h"
#include "filesystem.h"
#include "../../shared/include/logger.h"
#include "../../shared/include/io_tipos.h"

extern t_log *logger;
extern io_config config;

void inicializar_io(char* nombre_interfaz, char* path_config);

#endif