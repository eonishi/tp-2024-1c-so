#include "../include/configuracion.h"

io_config config;

void inicializar_configuracion(char* path_config){

    log_info(logger, "Cargando configuración desde archivo: %s", path_config);

    t_config* config_loader = config_create(path_config);

    if(config_loader == NULL) {
        log_error(logger, "No se encontro el archivo: [%s]", path_config);
        exit(EXIT_FAILURE);
    }

    // TODO: Validacion si el archivo de configuracion existe y contiene todo lo necesario
    // TODO: Archivo de configuracion para cada tipo de interfaz
    config.interfaz_tipo = config_get_string_value(config_loader, "TIPO_INTERFAZ");
    config.unidad_trabajo = config_get_int_value(config_loader, "TIEMPO_UNIDAD_TRABAJO");
    config.ip_kernel = config_get_string_value(config_loader, "IP_KERNEL");
    config.puerto_kernel = config_get_string_value(config_loader, "PUERTO_KERNEL");
    config.ip_memoria = config_get_string_value(config_loader, "IP_MEMORIA");
    config.puerto_memoria = config_get_string_value(config_loader, "PUERTO_MEMORIA");
    config.path_base_dialfs = config_get_string_value(config_loader, "PATH_BASE_DIALFS");
    config.block_size = config_get_int_value(config_loader, "BLOCK_SIZE");
    config.block_count = config_get_int_value(config_loader, "BLOCK_COUNT");
    config.retraso_compatacion = config_get_int_value(config_loader, "RETRASO_COMPACTACION");
}