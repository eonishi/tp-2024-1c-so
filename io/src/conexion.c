#include "../include/conexion.h"

int kernel_socket, memory_socket;

void crear_conexion_kernel(){
    kernel_socket = crear_conexion(config.ip_kernel, config.puerto_kernel);
    if(kernel_socket == -1){
        log_error(logger, "No se pudo conectar con el kernel");
        exit(EXIT_FAILURE);
    }
    log_info(logger, "Conexión con el kernel establecida");
}

void crear_conexion_memoria(){
    int memory_socket = crear_conexion(config.ip_memoria, config.puerto_memoria);
    if(memory_socket == -1){
        log_error(logger, "No se pudo conectar con la memoria");
        exit(EXIT_FAILURE);
    }
    log_info(logger, "Conexión con la memoria establecida");
}

 static int conexion_fue_exitosa(int socket){
    op_code status = recibir_operacion(socket);
    log_info(logger, "Status de la conexión: [%d]", status);

    if(status != SUCCESS){
        log_error(logger, "Ya existe una interfaz con ese nombre. Intente con otro nombre ;)");
        return 0;
    }
    return 1;
 }

int conectar_al_kernel(char* nombre_interfaz, io_tipo tipo){
    solicitud_conexion_kernel solicitud = {nombre_interfaz, tipo};
    enviar_solicitud_conexion_kernel(solicitud, kernel_socket);
    return conexion_fue_exitosa(kernel_socket);

}