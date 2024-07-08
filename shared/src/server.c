#include "../include/server.h"
#include <sys/socket.h>


int iniciar_servidor(char* server_name, char* ip, char* puerto)
{
	log_info(logger, "Iniciando servidor");
	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family,
                         servinfo->ai_socktype,
                         servinfo->ai_protocol);

	// Activamos la reutilización del puerto
	int activado = 1;
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	// Asociamos el socket a un puerto
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	// Escuchamos las conexiones entrantes
	listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);
	log_info(logger, "%s Listo para escuchar a mi cliente en %s:%s", server_name, ip, puerto);

	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{
	// Aceptamos un nuevo cliente
	int socket_cliente;
	socket_cliente = accept(socket_servidor, NULL, NULL);
	log_info(logger, "Se conecto un cliente. Socket: %d", socket_cliente);

	return socket_cliente;
}

int servidor_escuchar_cliente(int server_socket, void* (*function_handler)(void*)){
	int* cliente_socket_ptr = malloc(sizeof(int*));
	if(cliente_socket_ptr == NULL){
		log_error(logger, "No se pudo reservar memoria para el cliente");
		return 0;
	}

	log_info(logger, "Esperando cliente...");
	*cliente_socket_ptr = esperar_cliente(server_socket);
	if(*cliente_socket_ptr == -1){
		log_error(logger, "No se pudo conectar con el cliente");
		return 0;
	}

	if(esperar_handshake(*cliente_socket_ptr) == -1){
		log_error(logger, "No hubo un HANDSHAKE exitoso con el cliente");
		return 0;
	}
	else{
		log_info(logger, "HANDSHAKE exitoso con el cliente");
		enviar_handshake(*cliente_socket_ptr);
	}

	pthread_t thread;
	if(pthread_create(&thread, NULL, function_handler, (void*)cliente_socket_ptr) != 0){
		log_error(logger, "No se pudo crear el hilo para el cliente");
		close(*cliente_socket_ptr);
		free(cliente_socket_ptr);
		return 0;
	}
	else{
		pthread_detach(thread);
		return 1;
	}
}
