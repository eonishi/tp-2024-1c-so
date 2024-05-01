#include "include/cpu.h"

int main()
{
	logger = iniciar_logger("cpu.log", "CPU");
	log_info(logger, "Logger CPU Iniciado");

	inicializar_configuracion();  
  	crear_conexion_memoria();
	
	pthread_t server_threads[2];
	pthread_create(&server_threads[0], NULL, (void*)server_dispatch, NULL);
	pthread_create(&server_threads[1], NULL, (void*)server_interrupt, NULL);

	pthread_join(server_threads[0], NULL);
	pthread_join(server_threads[1], NULL);

	return EXIT_SUCCESS;
}

void inicializar_configuracion()
{
	log_info(logger, "Inicializando configuración...");

	t_config *config_loader = config_create("cpu.config");

	config.ip_cpu = config_get_string_value(config_loader, "IP_CPU");
	config.puerto_dispatch = config_get_string_value(config_loader, "PUERTO_ESCUCHA_DISPATCH");
	config.puerto_interrupt = config_get_string_value(config_loader, "PUERTO_ESCUCHA_INTERRUPT");

	config.ip_memoria = config_get_string_value(config_loader, "IP_MEMORIA");
	config.puerto_memoria = config_get_string_value(config_loader, "PUERTO_MEMORIA");

	config.cantidad_entradas_tlb = config_get_int_value(config_loader, "CANTIDAD_ENTRADAS_TLB");
	config.algoritmo_tlb = config_get_string_value(config_loader, "ALGORITMO_TLB");

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

void server_dispatch()
{
	int dispatch_fd = iniciar_servidor("CPU_DISPATCH", config.ip_cpu, config.puerto_dispatch);
  
  esperar_handshake_kernel(dispatch_fd);

	while (1)
	{
		log_trace(logger, "Estoy por recibir operacion");
		int cod_op = recibir_operacion(socket_kernel);
		log_info(logger, "Codigo recibido: %d", cod_op);

		switch (cod_op)
		{
		case DISPATCH_PROCESO:
			log_info(logger, "==============================================");
            log_info(logger, "DISPATCH_PROCESO recibido. CODIGO: %d", cod_op);

			pcb* pcb = recibir_pcb(socket_kernel);
			// Operaciones correspondientes
			enviar_mensaje(FETCH_INSTRUCCION, "X", socket_memoria);		
			log_info(logger, "Solicitud FETCH_INSTRUCCION enviada a memoria");
			esperar_respuesta(socket_memoria, FETCH_INSTRUCCION);

			log_info(logger, "Respuesta FETCH_INSTRUCCION recibida");
			// Más operaciones
			
			enviar_pcb(pcb, socket_kernel, DISPATCH_PROCESO);

			log_info(logger, "Fin DISPATCH_PROCESO");
			log_info(logger, "==============================================");
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			terminar_programa();
			exit(EXIT_FAILURE);
		default:
			log_warning(logger, "Operacion desconocida. No quieras meter la pata");
			break;
		}
		log_info(logger, "==============================================");
	}
}

void server_interrupt()
{
	int interrupt_fd = iniciar_servidor("CPU_INTERRUPT", config.ip_cpu, config.puerto_interrupt);
	int cliente_fd = esperar_cliente(interrupt_fd);

	while (1)
	{
		int cod_op = recibir_operacion(cliente_fd);
		log_info(logger, "Codigo recibido: %d", cod_op);

		switch (cod_op)
		{
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			enviar_mensaje(MENSAJE, "Respuesta de CPU", cliente_fd);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			terminar_programa();
			exit(EXIT_FAILURE);
		default:
			log_warning(logger, "Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
}

/*
El módulo CPU en nuestro contexto de TP lo que va a hacer es simular los pasos del ciclo de instrucción de una CPU real,
de una forma mucho más simplificada.
*/