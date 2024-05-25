#ifndef CLIENT_H_
#define CLIENT_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<commons/log.h>

extern t_log* logger;

int crear_conexion(char* ip, char* puerto);
void liberar_conexion(int socket_cliente);

#endif /* UTILS_H_ */
