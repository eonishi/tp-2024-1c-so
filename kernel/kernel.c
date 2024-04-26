#include "include/kernel.h"


pthread_t tid[2];

int main(){
    logger = iniciar_logger("kernel.log", "KERNEL");

    log_info(logger, "Logger de Kernel iniciado");

    inicializar_configuracion();
	iniciar_servidor_en_hilo();

	crear_conexion_memoria();
	crear_conexion_cpu();
    

	iniciar_consola();

	terminar_programa();

	return EXIT_SUCCESS;
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

	socket_cpu = conexion;

	enviar_handshake(socket_cpu);
	esperar_handshake(socket_cpu);

    return conexion;
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

void iniciar_consola()
{
	char* leido;
	leido = "void";
	bool ingresoActivado = 1;

	while(ingresoActivado){
		log_info(logger, "Ingrese INICIAR_PROCESO:");

		leido = readline("> ");
		log_info(logger, "Linea ingresada: %s", leido);

		if (strcmp(leido, "") == 0)
			ingresoActivado = 0;
			
		if(strcmp(leido, "INICIAR_PROCESO") == 0){
			log_info(logger, "==============================================");
			log_info(logger, "Inicio de ejecución de INICIAR_PROCESO");

			iniciar_proceso_en_memoria(socket_memoria);

			// Crear pcb.. etc

			dispatch_proceso(socket_cpu);
			log_info(logger, "Fin de ejecución de INICIAR_PROCESO");
			log_info(logger, "==============================================");

		}
		else if(strcmp(leido, "CPU") == 0){
			log_info(logger, "Enviando mensaje al CPU...");
    		enviar_mensaje(MENSAJE, "0", socket_cpu);
			log_info(logger, "Mensaje enviado");
		}
		else if(strcmp(leido, "MEMORIA") == 0) {
			log_info(logger, "Enviando mensaje a la MEMORIA...");
    		enviar_mensaje(MENSAJE,"0", socket_memoria);
			log_info(logger, "Mensaje enviado");
		}
		else{
			log_info(logger, "Comando desconocido");
		}

		free(leido);
	}
}

void iniciar_proceso_en_memoria(int conexion){
	enviar_mensaje(CREAR_PROCESO_EN_MEMORIA, "X", conexion);	
	log_info(logger, "Solicitud CREAR_PROCESO_EN_MEMORIA enviada a memoria");
	recibir_mensaje(conexion);
	log_info(logger, "Respuesta CREAR_PROCESO_EN_MEMORIA recibida");
}

void dispatch_proceso(int conexion){
	enviar_mensaje(DISPATCH_PROCESO, "X", conexion);	
	log_info(logger, "Solicitud DISPATCH_PROCESO enviada a CPU");
	recibir_mensaje(conexion);
	log_info(logger, "Respuesta DISPATCH_PROCESO recibida");
}

void terminar_programa()
{
    liberar_conexion(socket_cpu);
    liberar_conexion(socket_memoria);

    log_info(logger, "Memoria liberada correctamente");
    log_destroy(logger);
}

void *iniciar_escucha(){
	int server_socket = iniciar_servidor("KERNEL", config.ip_kernel, config.puerto_kernel);
	int cliente_fd = esperar_cliente(server_socket);

	t_list* lista;
	bool on = 1;
	while (on) {
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
			on = 0;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
}

void iniciar_servidor_en_hilo(){
	int err = pthread_create(&(tid[0]), NULL, iniciar_escucha, NULL);
    if (err != 0){
    	log_info(logger, "Hubo un problema al crear el thread de la escucha del servidor [%s]", strerror(err));
    }
	else{
    	log_info(logger, "El thread de la escucha del servidor inició su ejecución");
		pthread_detach(tid[0]);
	}
}
/*
El módulo Kernel, en el contexto de nuestro trabajo práctico, será el encargado de gestionar la ejecución de los 
diferentes procesos que se generen por medio de su consola interactiva.

*/