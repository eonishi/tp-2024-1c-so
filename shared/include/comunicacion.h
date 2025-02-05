#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<commons/collections/list.h>
#include<commons/log.h>
#include "paquete.h"
#include "codigos_operacion.h"

extern t_log* logger;
// Envios
void enviar_handshake(int socket);
void enviar_mensaje(int codigo_op, char* mensaje, int socket_cliente);
void enviar_status(int codigo_op, int socket_cliente);
void enviar_cantidad(unsigned cantidad, int codigo_op, int socket_cliente);
void enviar_buffer(void *buffer, int size, int socket_cliente);

// Esperas
int esperar_handshake(int socket);
int esperar_respuesta(int socket, op_code codigo_esperado);
t_list* recibir_paquete(int);
char* recibir_mensaje(int);
int recibir_operacion(int); // Sirve como recibir status
void* recibir_buffer(int*, int);
unsigned recibir_interrupcion(int);
unsigned deserializar_interrupcion(void*);
unsigned recibir_cantidad(int);

#endif