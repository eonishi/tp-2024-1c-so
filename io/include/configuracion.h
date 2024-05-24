#ifndef CONFIG_IO_H_
#define CONFIG_IO_H_

#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>


typedef struct{
    char *IP_KERNEL;
    char *PUERTO_KERNEL;
    char *IP_MEMORIA;
    char *PUERTO_MEMORIA;
    //char *TIPO_INTERFAZ;
    //unsigned TIEMPO_UNIDAD_TRABAJO;
    //char *PATH_BASE_DIALFS;
    //unsigned BLOCK_SIZE;
    //unsigned BLOCK_COUNT;
    //unsigned RETRASO_COMPACTACION;
} io_config;

extern t_log *logger;


io_config* inicializar_config();
bool cargar_configuracion(io_config* config);

#endif