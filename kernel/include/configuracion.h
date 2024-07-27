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
    char *puerto_cpu_dispatch;
    char* puerto_cpu_interrupt;
    char* algoritmo_planificacion;
    unsigned quantum;
    char** recursos;
    char** instancias_recursos;
    int grado_multiprogramacion;
} kernel_config;


extern t_log *logger;
extern t_log *logger_oblig;


kernel_config* inicializar_config();
bool cargar_configuracion(kernel_config* config);

#endif