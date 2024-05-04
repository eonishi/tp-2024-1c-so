#include "../include/configuracion.h"


char* CONFIG_FILE = "kernel.config";

kernel_config* inicializar_config(){
    log_info(logger, "Inicializando configuración...");

    kernel_config* config = malloc(sizeof(kernel_config));

    if((config) != NULL){
        (config)->ip_cpu = NULL;
        (config)->puerto_cpu = NULL;
        (config)->ip_kernel = NULL;
        (config)->puerto_kernel = NULL;
        (config)->ip_memoria = NULL;
        (config)->puerto_memoria = NULL;

        log_info(logger, "Configuración inicializada");

        return config;
    }

    log_error(logger, "Error al asignar memoria para la configuración");

    return NULL;
}

bool cargar_configuracion(kernel_config* config){
    log_info(logger, "Cargando configuración desde archivo: %s", CONFIG_FILE);

    t_config* config_loader = config_create(CONFIG_FILE);;

    if(config_loader == NULL) {
        log_error(logger, "No se encontro el archivo: [%s]", CONFIG_FILE);
        return false;
    }

    config->ip_cpu = config_get_string_value(config_loader, "IP_CPU");
    config->puerto_cpu = config_get_string_value(config_loader, "PUERTO_CPU");

    config->ip_memoria = config_get_string_value(config_loader, "IP_MEMORIA");
    config->puerto_memoria = config_get_string_value(config_loader, "PUERTO_MEMORIA");

    config->ip_kernel = config_get_string_value(config_loader, "IP_KERNEL");
    config->puerto_kernel = config_get_string_value(config_loader, "PUERTO_KERNEL");

    config->gradoMultiprogramacion = config_get_int_value(config_loader, "GRADOMULTIPROGRAMACION");

    log_info(logger, "IP_CPU: %s", config->ip_cpu);

    log_info(logger, "Configuración iniciadada correctamente.");

    return true;
}