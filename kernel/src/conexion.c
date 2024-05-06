#include "../include/conexion.h"

bool generar_conexiones(int* socket_cpu, int* socket_memoria){
    log_info(logger, "Creando conexión con CPU...");

    *socket_cpu = generar_conexion(config->ip_cpu, config->puerto_cpu_dispatch);

    if(*socket_cpu == -1)
        return false;

    log_info(logger, "Creando conexión con MEMORIA...");
    *socket_memoria = generar_conexion(config->ip_memoria, config->puerto_memoria);

    if(*socket_memoria == -1)
        return false;

    return true;
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