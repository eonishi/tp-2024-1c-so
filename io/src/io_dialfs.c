#include "../include/io_dialfs.h"

void io_dialfs() {
    inicializar_filesystem();

    log_info(logger, "Hola, soy IO DIALFS :)");

    while (kernel_socket != -1){
        imprimir_bitmap();

        int cod_op = recibir_operacion(kernel_socket);
        log_info(logger, "Codigo de operacion: [%d]", cod_op);

        switch (cod_op)
        {
            case CREAR_ARCHIVO_FS:
                log_info(logger, "Solicitud IO_FS_CREATE recibida");

                char* nombre_archivo = recibir_mensaje(kernel_socket);

                if(!crear_archivo(nombre_archivo)){
                    log_error(logger, "No se pudo crear el archivo");
                    // TODO: Enviar respuesta de error?
                }

                enviar_status(FIN_EJECUCION_IO, kernel_socket);
            break;
            case TRUNCAR_ARCHIVO_FS:
                log_info(logger, "Solicitud IO_FS_TRUNCATE recibida");
                
                solicitud_truncar_archivo solicitud_recibida = recibir_solicitud_truncar_archivo_fs(kernel_socket);

                if(!truncar_archivo(solicitud_recibida.nombre_archivo, solicitud_recibida.tamanio_archivo)){
                    log_error(logger, "No se pudo truncar el archivo");
                }

                enviar_status(FIN_EJECUCION_IO, kernel_socket);
            break;
            case -1:
                log_error(logger, "Se desconecto el kernel");
                exit(EXIT_FAILURE);
            break;
            default:
                log_error(logger, "Codigo de operacion no reconocido");
                break;
        }
    }
}

