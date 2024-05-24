#include "../include/configuracion.h"


char* CONFIG_FILE = "io.config";

io_config* inicializar_config(){
    log_info(logger, "Inicializando configuración...");

    io_config* config = malloc(sizeof(io_config));

    if((config) != NULL){
        (config)->IP_KERNEL = NULL;
        (config)->IP_MEMORIA = NULL;
        (config)->PUERTO_KERNEL = NULL;
        (config)->PUERTO_MEMORIA = NULL;

        log_info(logger, "Configuración inicializada");

        return config;
    }

    log_error(logger, "Error al asignar memoria para la configuración");

    return NULL;
}

bool cargar_configuracion(io_config* config){
    log_info(logger, "Cargando configuración desde archivo: %s", CONFIG_FILE);

    t_config* config_loader = config_create(CONFIG_FILE);;

    if(config_loader == NULL) {
        log_error(logger, "No se encontro el archivo: [%s]", CONFIG_FILE);
        return false;
    }

    config->IP_KERNEL = config_get_string_value(config_loader, "IP_KERNEL");
    config->PUERTO_KERNEL = config_get_string_value(config_loader, "PUERTO_KERNEL");

    config->IP_MEMORIA = config_get_string_value(config_loader, "IP_MEMORIA");
    config->PUERTO_MEMORIA = config_get_string_value(config_loader, "PUERTO_MEMORIA");

    log_info(logger, "Configuración iniciadada correctamente.");

    return true;
}