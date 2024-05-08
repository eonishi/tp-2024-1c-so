#include "../include/configuracion.h"

memory_config config;

void inicializar_configuracion(){
    log_info(logger, "Inicializando configuración...");

    t_config* config_loader = config_create("memoria.config");

    config.ip_memoria = config_get_string_value(config_loader, "IP_MEMORIA");
    config.puerto_memoria = config_get_string_value(config_loader, "PUERTO_ESCUCHA");
    config.tam_memoria = config_get_int_value(config_loader, "TAM_MEMORIA");
    config.tam_pagina = config_get_int_value(config_loader, "TAM_PAGINA");
    config.path_instrucciones = config_get_string_value(config_loader, "PATH_INSTRUCCIONES");
    config.retardo_memoria = config_get_int_value(config_loader, "RETARDO_RESPUESTA");

    log_info(logger, "Configuración iniciadada correctamente.");
}