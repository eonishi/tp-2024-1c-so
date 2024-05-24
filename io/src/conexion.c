#include "../include/conexion.h"

bool generar_conexiones(){
    log_info(logger, "Creando conexión con Kernel...");

    socket_kernel = generar_conexion(config->IP_KERNEL, config->PUERTO_KERNEL);

    if(socket_kernel == -1)
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