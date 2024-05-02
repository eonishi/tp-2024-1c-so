#include "../include/configuracion.h"

void inicializar_configuracion(){
    log_info(logger, "Inicializando configuración...");

    t_config* config_loader = config_create("kernel.config");;


    config.ip_cpu = config_get_string_value(config_loader, "IP_CPU");
    config.puerto_cpu = config_get_string_value(config_loader, "PUERTO_CPU");

    config.ip_memoria = config_get_string_value(config_loader, "IP_MEMORIA");
    config.puerto_memoria = config_get_string_value(config_loader, "PUERTO_MEMORIA");

    config.ip_kernel = config_get_string_value(config_loader, "IP_KERNEL");
    config.puerto_kernel = config_get_string_value(config_loader, "PUERTO_KERNEL");

    log_info(logger, "Configuración iniciadada correctamente.");
}