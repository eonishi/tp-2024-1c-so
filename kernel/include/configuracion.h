#ifndef CONFIG_KERNEL_H_
#define CONFIG_KERNEL_H_

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
extern kernel_config config;


void inicializar_configuracion();

#endif