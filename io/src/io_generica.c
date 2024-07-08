#include "../include/io_generica.h"

void io_generica() {
    log_info(logger, "Hola, soy IO generica :)");

    while (kernel_socket != -1){
        int cod_op = recibir_operacion(kernel_socket);
        log_info(logger, "Codigo de operacion: [%d]", cod_op);

        switch (cod_op){
            case EJECUTAR_INSTRUCCION_IO:
                log_info(logger, "Ejecutar instruccion IO");
                t_list *peticiones_memoria;
                char **instruccion_tokenizada = recibir_instruccion_io(kernel_socket, &peticiones_memoria);

                log_info(logger, "Instruccion: [%s]", instruccion_tokenizada[0]);
                log_info(logger, "Dispositivo: [%s]", instruccion_tokenizada[1]);
                log_info(logger, "Tiempo de retardo: [%s]", instruccion_tokenizada[2]);
                
                // ejecutar instruccion
                int tiempo_retardo = atoi(instruccion_tokenizada[2]) * config.unidad_trabajo * 1000;
                usleep(tiempo_retardo);
                
                // devolver status
                enviar_status(FIN_EJECUCION_IO, kernel_socket);
                log_info(logger, "Instruccion IO ejecutada");
                break;
                case -1:
                log_error(logger, "Se desconecto el kernel");
                exit(EXIT_FAILURE);
            default:
                log_error(logger, "Codigo de operacion no reconocido");
                break;
        }
    }
    
}
