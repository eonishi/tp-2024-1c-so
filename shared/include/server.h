#ifndef SERVER_H_
#define SERVER_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include <pthread.h>
#include "comunicacion.h"
#include<commons/log.h>

extern t_log* logger;

int iniciar_servidor(char*, char*, char*);
int esperar_cliente(int);
int servidor_escuchar_cliente(int server_socket, void *(*function_handler)(void *));

#endif /* SERVER_H_ */
