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
    char *puerto_memoria; //por consigna deberia ser numerico
    char *ip_cpu;
    char *puerto_cpu;
    int *gradoMultiprogramacion;
} kernel_config;//chequear los parametros y tipos minimos pedidos en la consigna


extern t_log *logger;


kernel_config* inicializar_config();
bool cargar_configuracion(kernel_config* config);

#endif