#include "../include/io_generica.h"

void io_generica() {
    log_info(logger, "IO generica");
    int cod_op = recibir_operacion(kernel_socket);

    while (kernel_socket != -1){
        switch (cod_op){
            case EJECUTAR_INTRUCCION_IO:
                log_info(logger, "Ejecutar instruccion IO");
                char** instruccion_tokenizada = recibir_instruccion_io(kernel_socket);

                int tiempo_retardo = atoi(instruccion_tokenizada[2]) * config.unidad_trabajo * 1000;
                usleep(tiempo_retardo);
                // devolver status
                enviar_status(kernel_socket, SUCCESS);
                break;
            default:
                log_error(logger, "Codigo de operacion no reconocido");
                break;
        }
    }
    
}