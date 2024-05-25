
#ifndef CONFIGURACION_IO_H_
#define CONFIGURACION_IO_H_

#include <commons/config.h>
#include <commons/log.h>

extern t_log *logger;

typedef struct 
{
    char *interfaz_tipo;
    unsigned unidad_trabajo; // Tiempo numerico
    char *ip_kernel;
    char* puerto_kernel;
    char* ip_memoria;
    char* puerto_memoria;
    char *path_base_dialfs;
    unsigned block_size;
    unsigned block_count;
    unsigned retraso_compatacion;
} io_config;

void inicializar_configuracion(char*);


#endif