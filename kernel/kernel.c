#include "include/kernel.h"


int main(){
    logger = iniciar_logger("kernel.log", "KERNEL");

    log_info(logger, "Logger de Kernel iniciado");

    inicializar_configuracion();

	int conexion_cpu = crear_conexion_cpu();
    int conexion_memoria = crear_conexion_memoria();

    log_info(logger, "Enviando mensaje al CPU...");
    enviar_mensaje("0", conexion_cpu);
    log_info(logger, "Mensaje enviado");
    

    log_info(logger, "Enviando mensaje a la memoria...");
    enviar_mensaje("0", conexion_memoria);
    log_info(logger, "Mensaje enviado");

	// Luego de haber conectado con sus servidores, levanto el servidor de Kernel para la conexion con IO
	int server_fd = iniciar_servidor("KERNEL", config.ip_kernel, config.puerto_kernel);
	int cliente_fd = esperar_cliente(server_fd);

	t_list* lista;
	while (1) {
        log_info(logger, "Estoy por recibir operacion");
		int cod_op = recibir_operacion(cliente_fd);
        log_info(logger, "Codigo recibido:");

		switch (cod_op) {
		case MENSAJE:
            log_info(logger, "Entre a MENSAJE. CODIGO: %d", cod_op);
			recibir_mensaje(cliente_fd);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return EXIT_SUCCESS;

    return 0;
}

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

void iterator(char* value) {
	log_info(logger,"%s", value);
}

int crear_conexion_cpu()
{
    log_info(logger, "Creando conexión con CPU...");
	int conexion = crear_conexion(config.ip_cpu, config.puerto_cpu);
	log_info(logger, "Conexión creada. Socket: %i", conexion);

    return conexion;
}

int crear_conexion_memoria()
{
    log_info(logger, "Creando conexión con Memoria...");
	int conexion = crear_conexion(config.ip_memoria, config.puerto_memoria);
	log_info(logger, "Conexión creada. Socket: %i", conexion);

    return conexion;
}
/*
El módulo Kernel, en el contexto de nuestro trabajo práctico, será el encargado de gestionar la ejecución de los 
diferentes procesos que se generen por medio de su consola interactiva.

*/