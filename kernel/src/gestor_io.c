#include "../include/gestor_io.h"

bool existe_io_conectada(char* nombre_io){
    return true;
}

conexion_io* obtener_conexion_io_por_nombre(char* nombre_io){
    return (conexion_io*) list_get(lista_conexiones_io, 0);
}

bool io_acepta_operacion(conexion_io conexion_io,char* operacion_io){
    return true;
}

bool io_disponible(conexion_io conexion_io){
    return true;
}


bool validar_y_enviar_instruccion_a_io(char** instruc_io_tokenizadas){
    conexion_io* conexion_io;

    if(!existe_io_conectada(instruc_io_tokenizadas[1])){
        log_error(logger, "La IO no existe: [%s]", instruc_io_tokenizadas[0]);

        return false;
    }

    log_info(logger, "La io existe");
    conexion_io = obtener_conexion_io_por_nombre(instruc_io_tokenizadas[1]);

    if(!io_acepta_operacion(*conexion_io, instruc_io_tokenizadas[0])){
        log_error(logger, "La IO no existe: [%s]", instruc_io_tokenizadas[0]);
    }

    log_info(logger, "Y la operación es válida.");

    if(!io_disponible(*conexion_io)){
        log_error(logger, "La IO no está disponible");
    }

    enviar_instruccion_io(instruc_io_tokenizadas, conexion_io->socket);

    return true;
}