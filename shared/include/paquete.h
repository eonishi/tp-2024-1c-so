#ifndef PAQUETE_H_
#define PAQUETE_H_

#include<stdlib.h>
#include<sys/socket.h>
#include<string.h>
#include "protocolo.h"

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;


void* serializar_paquete(t_paquete* paquete, int bytes);
t_paquete* crear_paquete(op_code codigo_operacion);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void crear_buffer(t_paquete* paquete);

#endif