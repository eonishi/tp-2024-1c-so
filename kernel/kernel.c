#include "include/kernel.h"


pthread_t tid[2];

int main(){
    logger = iniciar_logger("kernel.log", "KERNEL");

    log_info(logger, "Logger de Kernel iniciado");

    inicializar_configuracion();
	iniciar_servidor_en_hilo();

	int conexion_cpu = crear_conexion_cpu();
    int conexion_memoria = crear_conexion_memoria();

	iniciar_consola(conexion_cpu, conexion_memoria);

	terminar_programa(conexion_cpu, conexion_memoria);

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

    return conexion;
}

int crear_conexion_memoria()
{
    log_info(logger, "Creando conexión con Memoria...");
	int conexion = crear_conexion(config.ip_memoria, config.puerto_memoria);
	log_info(logger, "Conexión creada. Socket: %i", conexion);

    return conexion;
}

void iniciar_consola(int conexion_cpu, int conexion_memoria)
{
	char* leido;
	leido = "void";
	bool ingresoActivado = 1;

	while(ingresoActivado){
		log_info(logger, "Ingrese CPU o MEMORIA para enviar mensajes a esos modulos: ");

		leido = readline("> ");
		log_info(logger, "Linea ingresada: %s", leido);

		if (strcmp(leido, "") == 0)
			ingresoActivado = 0;
			
		if(strcmp(leido, "INICIAR_PROCESO") == 0){
			log_info(logger, "Enviando mensaje al INICIAR_PROCESO...");

			iniciar_proceso_en_memoria(conexion_memoria);

			// Crear pcb.. etc

			dispatch_proceso(conexion_cpu);

		}
		else if(strcmp(leido, "CPU") == 0){
			log_info(logger, "Enviando mensaje al CPU...");
    		enviar_mensaje("0", conexion_cpu);
			log_info(logger, "Mensaje enviado");
		}
		else if(strcmp(leido, "MEMORIA") == 0) {
			log_info(logger, "Enviando mensaje a la MEMORIA...");
    		enviar_mensaje("0", conexion_memoria);
			log_info(logger, "Mensaje enviado");
		}
		else{
			log_info(logger, "Comando desconocido");
		}

		free(leido);
	}
	// ¡No te olvides de liberar las lineas antes de regresar! // TODO Preguntar que onda esto.
}

void iniciar_proceso_en_memoria(int conexion){
	enviar_mensaje("X", conexion);	
	log_info(logger, "Mensaje enviado a memoria");
	recibir_mensaje(conexion);
}

void dispatch_proceso(int conexion){
	enviar_mensaje("X", conexion);	
	log_info(logger, "Mensaje enviado al cpu");
	recibir_mensaje(conexion);
}

void terminar_programa(int conexion_cpu, int conexion_memoria)
{
    liberar_conexion(conexion_cpu);
    liberar_conexion(conexion_memoria);
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