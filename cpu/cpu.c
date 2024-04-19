#include "include/cpu.h"

int main(){
    logger = iniciar_logger("cpu.log", "CPU");
	log_info(logger, "Logger CPU Iniciado");

	inicializar_configuracion();

	int server_fd = iniciar_servidor("CPU", config.ip_cpu, config.puerto_cpu);
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
}

void inicializar_configuracion(){
    log_info(logger, "Inicializando configuración...");
    // config = malloc(sizeof(kernel_config));

    t_config* config_loader = config_create("cpu.config");;

    config.ip_cpu = config_get_string_value(config_loader, "IP_CPU");
    config.puerto_cpu = config_get_string_value(config_loader, "PUERTO_CPU");

    config.ip_memoria = config_get_string_value(config_loader, "IP_MEMORIA");
    config.puerto_memoria = config_get_string_value(config_loader, "PUERTO_MEMORIA");

    log_info(logger, "Configuración iniciadada correctamente.");
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}

/*
El módulo CPU en nuestro contexto de TP lo que va a hacer es simular los pasos del ciclo de instrucción de una CPU real, 
de una forma mucho más simplificada.
*/