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

void enviar_mensaje(int codigo_op, char* mensaje, int socket_cliente)
{
	log_info(logger,"Enviando cod: [%d] mensaje: [%s]", codigo_op, mensaje);
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = codigo_op;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	log_info(logger, "Apunto de serializar");
	void* a_enviar = serializar_paquete(paquete, bytes);
	log_info(logger, "Serializado");

	send(socket_cliente, a_enviar, bytes, 0);


	free(a_enviar);
	eliminar_paquete(paquete);
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}


int esperar_respuesta(int socket, op_code codigo_esperado){
	int codigo_recibido = recibir_operacion(socket);
	log_info(logger, "Esperar_respuesta: Codigo: [%d]", codigo_recibido);
    if(codigo_recibido == codigo_esperado) {
        recibir_mensaje(socket);

        return 0;
    }

    log_error(logger, "El código de respuesta no es el esperado. Esperado: [%d]. Recibido: [%d]", codigo_esperado, codigo_recibido);

	return ERROR;
}