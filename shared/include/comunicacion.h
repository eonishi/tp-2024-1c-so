#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include "server.h"
#include "paquete.h"

// Envios
void enviar_handshake(int socket);
void enviar_mensaje(int codigo_op, char* mensaje, int socket_cliente);

// Esperas
int esperar_handshake(int socket);
int esperar_respuesta(int socket, op_code codigo_esperado);

#endif