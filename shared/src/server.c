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
	log_info(logger, "Se conecto un cliente");

	return socket_cliente;
}
