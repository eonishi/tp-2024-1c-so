#include "include/kernel.h"

int main(){
    logger = iniciar_logger("kernel.log", "KERNEL");

    log_info(logger, "Logger de Kernel iniciado");

    // 

	int conexion;
	char* ip;
	char* puerto;
	char* valor;
    t_config* config = config_create("kernel.config");;

    log_info(logger, "Iniciando configuración...");


    char * clave = config_get_string_value(config, "CLAVE");
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");
	valor = config_get_string_value(config, "VALOR");


	// Loggeamos el valor de config

	log_info(logger, "La clave es: %s", clave);
	log_info(logger, "La ip es: %s", ip);
	log_info(logger, "La puerto es: %s", puerto);
	log_info(logger, "La valor es: %s", valor);

    log_info(logger, "Creando conexión...");
	conexion = crear_conexion(ip, puerto);
	log_info(logger, "Conexión creada. Socket: %i", conexion);

    enviar_mensaje("0", conexion);
    

    return 0;
}

/*
El módulo Kernel, en el contexto de nuestro trabajo práctico, será el encargado de gestionar la ejecución de los 
diferentes procesos que se generen por medio de su consola interactiva.

*/