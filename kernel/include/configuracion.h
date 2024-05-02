#ifndef CONFIG_KERNEL_H_
#define CONFIG_KERNEL_H_

#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>


typedef struct
{
    char *ip_kernel;
    char *puerto_kernel;
    char *ip_memoria;
    char *puerto_memoria;
    char *ip_cpu;
    char *puerto_cpu;
} kernel_config;


extern t_log *logger;


kernel_config* inicializar_config();
bool cargar_configuracion(kernel_config* config);

#endif