#include "../include/io_dialfs.h"

void io_dialfs() {
    log_info(logger, "IO DIALSFS iniciada");

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
            case ELIMINAR_ARCHIVO_FS:
                log_info(logger, "Solicitud IO_FS_DELETE recibida");

                char* nombre_archivo_a_eliminar = recibir_mensaje(kernel_socket);

                if(!eliminar_archivo(nombre_archivo_a_eliminar)){
                    log_error(logger, "No se pudo crear el archivo");
                    // TODO: Enviar respuesta de error?
                }

                enviar_status(FIN_EJECUCION_IO, kernel_socket);
            break;
            case EJECUTAR_INSTRUCCION_IO:
                    t_list *peticiones_memoria;
                    char** tokens_instr = recibir_instruccion_io(kernel_socket, &peticiones_memoria);
                    log_info(logger, "Instruccion recibida de Kernel [%s]", tokens_instr[0]);
                    log_peticiones(peticiones_memoria);

                    // [] Enviar peticiones a memoria y guardar el resultado
                    size_t tam_total = peticiones_tam_total(peticiones_memoria);
                    log_info(logger, "Tam total de las peticiones [%d]", tam_total);
                    void* datos = malloc(tam_total);
                    void* ptr_string = datos;

                    for (int i = 0; i < list_size(peticiones_memoria); i++){
                         t_peticion_memoria* peticion = list_get(peticiones_memoria, i);
                         log_info(logger, "Peticion [%d] de [%d]", i+1, list_size(peticiones_memoria));
                         peticion_lectura_enviar(peticion, &ptr_string, memory_socket);
                         controlar_peticion();
                         log_info(logger, "Peticion [%d] Recibido: [%s]", i+1, datos);
                    }

                    log_info(logger, "Leido desde memoria: [%s]", datos);

                    char* nombre_archivo_escribir = tokens_instr[2];
                    char* puntero_archivo = tokens_instr[4];
                    if(!escribir_archivo(nombre_archivo_escribir, datos, tam_total, atoi(puntero_archivo))){
                        log_error(logger, "No se pudo escribir el archivo");

                        // TODO: Enviar respuesta de error?
                    }

                    log_info(logger, "Se ha escrito correctamente el archivo: [%s]", nombre_archivo_escribir);

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

static void controlar_peticion(){
     op_code status = recibir_operacion(memory_socket);
     if(status == SUCCESS){
        log_info(logger, "La operacion en memoria fue exitosa");
     }
     else{
        log_error(logger, "Hubo un problema con la operacion en memoria");
     }
}

