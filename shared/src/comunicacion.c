#include "../include/comunicacion.h"

void enviar_handshake(int socket) {
    enviar_mensaje(HANDSHAKE, "HANDSHAKE", socket);
}

int esperar_handshake(int socket) {
    int cod_op = recibir_operacion(socket);
    log_info(logger, "esperar_handshake. Codigo: [%d]", cod_op);
    if(cod_op == HANDSHAKE) {
        recibir_mensaje(socket);

        return 0;
    }

    return ERROR;
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

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);


	free(a_enviar);
	eliminar_paquete(paquete);
}
void enviar_status(int codigo_op, int socket_cliente)
{
	void * buffer = malloc(sizeof(int));
	memcpy(buffer, &codigo_op, sizeof(int));

	send(socket_cliente, buffer, sizeof(int), 0);
	free(buffer);
}

void enviar_cantidad(unsigned cantidad ,int codigo_op, int socket_cliente)
{
	int size_buffer = sizeof(int) + sizeof(unsigned);
	void * buffer = malloc(size_buffer);
	int desplazamiento = 0;

	memcpy(buffer, &codigo_op, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(buffer + desplazamiento, &cantidad, sizeof(unsigned));

	send(socket_cliente, buffer, size_buffer, 0);
	free(buffer);
}

unsigned recibir_cantidad(int socket_cliente){
	unsigned cantidad;
	if(recv(socket_cliente, &cantidad, sizeof(unsigned), MSG_WAITALL) != -1){
		return cantidad;
	}
	else{
		log_info(logger, "Error al recibir cantidad. Me Desconecto :(");
		close(socket_cliente);
		return -1;
	};
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



int recibir_operacion(int socket_cliente)
{
	int cod_op;

	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Mensaje recibido: [%s]", buffer); // TODO  REVISAR
	free(buffer);
}

char* recibir_respuesta(int socket){
	int size;
	char* buffer = recibir_buffer(&size, socket);
	return buffer;
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}


unsigned recibir_interrupcion(int socket_cliente)
{
	unsigned pid;
	void* buffer = malloc(sizeof(unsigned int));
	recv(socket_cliente, buffer, sizeof(unsigned int),0);
	pid = deserializar_interrupcion(buffer);
	free(buffer);
	if(pid > 0)
		return pid;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

unsigned deserializar_interrupcion(void* buffer) {
    unsigned valor;
    memcpy(&valor, buffer, sizeof(unsigned int));
    return valor;
}