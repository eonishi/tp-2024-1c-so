#include "../include/configuracion.h"


char* CONFIG_FILE = "kernel.config";
char* IP_CONFIG_FILE = "ip.config";

kernel_config* inicializar_config(){
    log_info(logger, "Inicializando configuración...");

    kernel_config* config = malloc(sizeof(kernel_config));

    if((config) != NULL){
        (config)->ip_cpu = NULL;
        (config)->puerto_cpu_dispatch = NULL;
        (config)->puerto_cpu_interrupt = NULL;
        (config)->ip_kernel = NULL;
        (config)->puerto_kernel = NULL;
        (config)->ip_memoria = NULL;
        (config)->puerto_memoria = NULL;
        (config)->grado_multiprogramacion = 0;
        (config)->algoritmo_planificacion = NULL;

        log_info(logger, "Configuración inicializada");

        return config;
    }

    log_error(logger, "Error al asignar memoria para la configuración");

    return NULL;
}

bool cargar_configuracion(kernel_config* config){
    log_info(logger, "Cargando configuración desde archivo: %s", CONFIG_FILE);

    t_config* ip_config_loader = config_create(IP_CONFIG_FILE);;

    if(ip_config_loader == NULL) {
        log_error(logger, "No se encontro el archivo: [%s]", IP_CONFIG_FILE);
        return false;
    }

    config->ip_kernel = config_get_string_value(ip_config_loader, "IP_KERNEL");
    config->puerto_kernel = config_get_string_value(ip_config_loader, "PUERTO_ESCUCHA");

    config->ip_memoria = config_get_string_value(ip_config_loader, "IP_MEMORIA");
    config->puerto_memoria = config_get_string_value(ip_config_loader, "PUERTO_MEMORIA");

    config->ip_cpu = config_get_string_value(ip_config_loader, "IP_CPU");
    config->puerto_cpu_dispatch = config_get_string_value(ip_config_loader, "PUERTO_CPU_DISPATCH");
    config->puerto_cpu_interrupt = config_get_string_value(ip_config_loader, "PUERTO_CPU_INTERRUPT");

    
    t_config* config_loader = config_create(CONFIG_FILE);;

    if(config_loader == NULL) {
        log_error(logger, "No se encontro el archivo: [%s]", CONFIG_FILE);
        return false;
    }

    config->algoritmo_planificacion = config_get_string_value(config_loader, "ALGORITMO_PLANIFICACION");
    config->quantum = config_get_int_value(config_loader, "QUANTUM");
    config->recursos = config_get_array_value(config_loader, "RECURSOS"); // A chequear si anda esto 🤔
    config->instancias_recursos = config_get_array_value(config_loader, "INSTANCIAS_RECURSOS");

    config->grado_multiprogramacion = config_get_int_value(config_loader, "GRADO_MULTIPROGRAMACION");

    log_info(logger, "IP_CPU: %s", config->ip_cpu);

    // Con config destroy no levanta...
    // config_destroy(ip_config_loader);
    // config_destroy(config_loader);        

    log_info(logger, "Configuración iniciadada correctamente.");

    return true;
}