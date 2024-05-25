#ifndef INICIALIZAR_IO_H_
#define INICIALIZAR_IO_H_

#include <commons/string.h> 
#include "configuracion.h"
#include "conexion.h"
#include "io_generica.h"
#include "../../shared/include/logger.h"
#include "../../shared/include/io_tipos.h"

void inicializar_io(char* nombre_interfaz, char* path_config);
#endif