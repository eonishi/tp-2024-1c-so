#include "include/cpu.h"

int main(){
    logger = iniciar_logger("cpu.log", "CPU");
	log_info(logger, "Logger CPU Iniciado");

	inicializar_configuracion();
	crear_conexion_memoria();

	int server_fd = iniciar_servidor("CPU", config.ip_cpu, config.puerto_cpu);
	esperar_handshake_kernel(server_fd);

	t_list* lista;

	log_info(logger, "Esperando operación...");
	while (1) {    
		int cod_op = recibir_operacion(socket_kernel);
		log_info(logger, "==============================================");

		switch (cod_op) {
		case DISPATCH_PROCESO:
            log_info(logger, "DISPATCH_PROCESO recibido. CODIGO: %d", cod_op);

			recibir_mensaje(socket_kernel);
			// Operaciones correspondientes
			enviar_mensaje(FETCH_INSTRUCCION, "Solicitud FETCH_INSTRUCCION desde CPU", socket_memoria);		
			log_info(logger, "Solicitud FETCH_INSTRUCCION enviada a memoria");
			recibir_mensaje(socket_memoria);
			log_info(logger, "Respuesta FETCH_INSTRUCCION recibida");
			// Más operaciones
			enviar_mensaje(RESPUESTA, "Respuesta DISPATCH_PROCESO de CPU", socket_kernel);
			break;
		case PAQUETE:
			lista = recibir_paquete(socket_kernel);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
		
			terminar_programa();

			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
		log_info(logger, "==============================================");
	}

	return EXIT_SUCCESS;
}

void inicializar_configuracion(){
    log_info(logger, "Inicializando configuración...");

    t_config* config_loader = config_create("cpu.config");;

    config.ip_cpu = config_get_string_value(config_loader, "IP_CPU");
    config.puerto_cpu = config_get_string_value(config_loader, "PUERTO_CPU");

    config.ip_memoria = config_get_string_value(config_loader, "IP_MEMORIA");
    config.puerto_memoria = config_get_string_value(config_loader, "PUERTO_MEMORIA");

    log_info(logger, "Configuración iniciadada correctamente.");
}

int crear_conexion_memoria()
{
    log_info(logger, "Creando conexión con Memoria...");
	int conexion = crear_conexion(config.ip_memoria, config.puerto_memoria);
	log_info(logger, "Conexión creada. Socket: %i", conexion);
	socket_memoria = conexion;

	enviar_handshake(socket_memoria);
	esperar_handshake(socket_memoria);

    return conexion;
}

void esperar_handshake_kernel(int server) {
    log_info(logger,"Esperando conexión del modulo Kernel ... ");
    socket_kernel = esperar_cliente(server);
	log_info(logger,"Esperando handshake del modulo Kernel ... ");
    int resultado = esperar_handshake(socket_kernel);
    if(resultado == -1) {
        log_error(logger,"No se pudo conectar con el modulo KERNEL");
        exit(EXIT_FAILURE);
    }

	log_info(logger,"Respondiendo handshake del modulo Kernel ... ");
    enviar_handshake(socket_kernel);
}

void terminar_programa()
{
    log_info(logger, "Memoria liberada correctamente");
    log_destroy(logger);
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}

/*
El módulo CPU en nuestro contexto de TP lo que va a hacer es simular los pasos del ciclo de instrucción de una CPU real, 
de una forma mucho más simplificada.
*/