#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include "server.h"
#include "client.h"

void enviar_handshake(int socket);
int esperar_handshake(int socket);

#endif