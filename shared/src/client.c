#include "../include/client.h"


int crear_conexion(char *ip, char* puerto)
{
	log_info(logger, "Iniciando conexión con servidor");

	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if(getaddrinfo(ip, puerto, &hints, &server_info) != 0){
		log_error(logger, "Error en getaddrinfo");
		return -1;
	}
	

	// Ahora vamos a crear el socket.
	int socket_cliente = socket(server_info->ai_family,
                    			server_info->ai_socktype,
                    			server_info->ai_protocol);

	if(socket_cliente == -1){
		log_error(logger, "Error al crear el socket");
		return -1;
	}

	// Ahora que tenemos el socket, vamos a conectarlo
	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1){
		log_error(logger, "Error al conectar el socket");
		return -1;
	}

	freeaddrinfo(server_info);
	return socket_cliente;
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}