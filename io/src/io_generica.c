#include "../include/io_generica.h"

void io_generica() {
    log_info(logger, "Hola, soy IO generica :)");

    while (kernel_socket != -1){
        int cod_op = recibir_operacion(kernel_socket);
        log_info(logger, "Codigo de operacion: [%d]", cod_op);

        switch (cod_op){
            case EJECUTAR_INSTRUCCION_IO:
                log_info(logger, "Ejecutar instruccion IO");
                
                solicitud_io_sleep parametros = recibir_io_sleep(kernel_socket);
                int retardo = parametros.retraso;
                int pid = parametros.pid;

                log_info(logger, "Se ejecuta IO_SLEEP par el proceos pid:[%d]", pid);
                // int retardo = recibir_cantidad(kernel_socket);
                
                // ejecutar instruccion
                int tiempo_retardo = retardo * config.unidad_trabajo * 1000;
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
