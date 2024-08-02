#include "../include/configuracion.h"

cpu_config config;

void inicializar_configuracion()
{
	log_info(logger, "Inicializando configuración...");

	t_config *ip_config_loader = config_create("ip.config");
	
    // Validacion si el archivo de configuracion existe y contiene todo lo necesario

	config.ip_cpu = config_get_string_value(ip_config_loader, "IP_CPU");
	config.puerto_dispatch = config_get_string_value(ip_config_loader, "PUERTO_ESCUCHA_DISPATCH");
	config.puerto_interrupt = config_get_string_value(ip_config_loader, "PUERTO_ESCUCHA_INTERRUPT");

	config.ip_memoria = config_get_string_value(ip_config_loader, "IP_MEMORIA");
	config.puerto_memoria = config_get_string_value(ip_config_loader, "PUERTO_MEMORIA");

	t_config *config_loader = config_create("cpu.config");

	config.cantidad_entradas_tlb = config_get_int_value(config_loader, "CANTIDAD_ENTRADAS_TLB");
	config.algoritmo_tlb = config_get_string_value(config_loader, "ALGORITMO_TLB");

	log_info(logger, "Configuración iniciadada correctamente.");
}