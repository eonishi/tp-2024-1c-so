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

	log_info(logger, "Apunto de serializar");
	void* a_enviar = serializar_paquete(paquete, bytes);
	log_info(logger, "Serializado");

	send(socket_cliente, a_enviar, bytes, 0);


	free(a_enviar);
	eliminar_paquete(paquete);
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