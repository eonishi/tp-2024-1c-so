#ifndef SERVER_H_
#define SERVER_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netdb.h>
#include<commons/log.h>

extern t_log* logger;

int iniciar_servidor(char*, char*, char*);
int esperar_cliente(int);

#endif /* SERVER_H_ */
