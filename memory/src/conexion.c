#include "../include/conexion.h"
int socket_cpu, socket_kernel;

void esperar_handshake_cpu(int server) {
    log_info(logger,"Esperando conexión del modulo CPU ... ");
    socket_cpu = esperar_cliente(server);
    log_info(logger, "SOCKET_CPU: [%d]", socket_cpu);
	log_info(logger,"Esperando handshake del modulo CPU ... ");
    int resultado = esperar_handshake(socket_cpu);
    if(resultado == -1) {
        log_error(logger,"No se pudo conectar con el modulo CPU");
        abort();
    }

    // Seria el handshake de la memoria
    log_info(logger,"Respondiendo handshake del modulo CPU ... ");
    enviar_cantidad(config.tam_pagina, HANDSHAKE, socket_cpu);
}

void esperar_handshake_kernel(int server) {
    log_info(logger,"Esperando conexión del modulo Kernel ... ");
    socket_kernel = esperar_cliente(server);
    log_info(logger, "socket_kernel: [%d]", socket_kernel);
	log_info(logger,"Esperando handshake del modulo Kernel ... ");
    int resultado = esperar_handshake(socket_kernel);
    if(resultado == -1) {
        log_error(logger,"No se pudo conectar con el modulo KERNEL");
        exit(EXIT_FAILURE);
    }

    log_info(logger,"Respondiendo handshake del modulo Kernel ... ");
    enviar_handshake(socket_kernel);
}