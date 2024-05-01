#ifndef CLIENT_H_
#define CLIENT_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include "protocolo.h"
#include "paquete.h"


extern t_log* logger;



int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(int codigo_op, char* mensaje, int socket_cliente);
void liberar_conexion(int socket_cliente);
int esperar_respuesta(int socket, op_code codigo_esperado);

#endif /* UTILS_H_ */
