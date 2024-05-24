#include "../include/conexion.h"

bool generar_conexiones(){
    log_info(logger, "Creando conexión con CPU...");

    socket_cpu_dispatch = generar_conexion(config->ip_cpu, config->puerto_cpu_dispatch);

    if(socket_cpu_dispatch == -1)
        return false;

    log_info(logger, "Creando conexión con MEMORIA...");
    socket_memoria = generar_conexion(config->ip_memoria, config->puerto_memoria);

    if(socket_memoria == -1)
        return false;

    log_info(logger, "Creando conexión con IO...");
    
    int server_socket = iniciar_servidor("KERNEL", config->ip_kernel, config->puerto_kernel);
	socket_io = recibir_conexion_io(server_socket);

    if(socket_io == -1)
        return false;

    return true;
}

int recibir_conexion_io(int server) {
    log_info(logger,"Esperando conexión del modulo IO ... ");
    int socket = esperar_cliente(server);
	log_info(logger,"Esperando handshake del modulo IO ... ");
    int resultado = esperar_handshake(socket);
    if(resultado == -1) {
        log_error(logger,"No se pudo conectar con el modulo IO");
        exit(EXIT_FAILURE);
    }

    log_info(logger,"Respondiendo handshake del modulo IO... ");
    enviar_handshake(socket);

    return socket;
}

int generar_conexion(char* ip, char* puerto){
	int conexion = crear_conexion(ip, puerto);
    if(conexion == -1){
        log_error(logger, "No se pudo conectar al servidor. IP: [%s] Puerto: [%s] Socket:[%d]", ip, puerto, conexion);    
        return -1;
    }

	log_info(logger, "Conexión creada. Socket: %i", conexion);

	enviar_handshake(conexion);
	if(esperar_handshake(conexion) == -1){
        log_error(logger, "Respuesta erronea de handshake");
    };

    log_info(logger, "Handshake realizado enviado y recibido correctamente");

    return conexion;
}