#include "../include/io_dialfs.h"

static void exec_io_write_fs(int pid, char **tokens_instr, t_list *peticiones_memoria);
static void exec_io_read_fs(int pid, char **tokens_instr, t_list *peticiones_memoria);

void io_dialfs() {
    log_info(logger, "IO DIALSFS iniciada");

    while (kernel_socket != -1){
        imprimir_bitmap();

        int cod_op = recibir_operacion(kernel_socket);
        log_info(logger, "Codigo de operacion: [%d]", cod_op);

        switch (cod_op)
        {
            case CREAR_ARCHIVO_FS:
                solicitud_accion_archivo solicitud_create = recibir_solicitud_accion_archivo_fs(kernel_socket);
            
                log_info(logger, "PID: <%d> - Crear Archivo: <%s>", solicitud_create.pid, solicitud_create.nombre_archivo);

                if(!crear_archivo(solicitud_create.nombre_archivo)){
                    log_error(logger, "No se pudo crear el archivo");
                    // TODO: Enviar respuesta de error?
                }

                enviar_status(FIN_EJECUCION_IO, kernel_socket);
            break;
            case TRUNCAR_ARCHIVO_FS:
                solicitud_truncar_archivo solicitud_recibida = recibir_solicitud_truncar_archivo_fs(kernel_socket);
            
                log_info(logger, "PID: <%d> - Truncar Archivo: <%s> - Tamaño: <%d>", 
                solicitud_recibida.pid, solicitud_recibida.nombre_archivo, solicitud_recibida.tamanio_archivo);

                if(!truncar_archivo(solicitud_recibida.pid, solicitud_recibida.nombre_archivo, solicitud_recibida.tamanio_archivo)){
                    log_error(logger, "No se pudo truncar el archivo");
                }

                enviar_status(FIN_EJECUCION_IO, kernel_socket);
            break;
            case ELIMINAR_ARCHIVO_FS:
                log_info(logger, "Solicitud IO_FS_DELETE recibida");

                solicitud_accion_archivo solicitud_delete = recibir_solicitud_accion_archivo_fs(kernel_socket);

                log_info(logger, "PID: <%d> - Eliminar Archivo: <%s>", solicitud_delete.pid, solicitud_delete.nombre_archivo);

                if(!eliminar_archivo(solicitud_delete.nombre_archivo)){
                    log_error(logger, "No se pudo crear el archivo");
                    // TODO: Enviar respuesta de error?
                }

                enviar_status(FIN_EJECUCION_IO, kernel_socket);
            break;
            case EJECUTAR_INSTRUCCION_IO:                    
                    solicitud_instruccion_io solicitud_instruccion = recibir_instruccion_io(kernel_socket);
                    int pid = solicitud_instruccion.pid;
                    t_list *peticiones_memoria = solicitud_instruccion.peticiones_memoria;
                    char** tokens_instr = solicitud_instruccion.tokens;

                    log_info(logger,"RECIBIDO EJECUTAR_INSTRUCCION_IO, pid: [%d]", pid);

                    log_info(logger, "Instruccion recibida de Kernel [%s]", tokens_instr[0]);
                    log_peticiones(peticiones_memoria);

                    if(strcmp(tokens_instr[0], "IO_FS_WRITE") == 0){
                        exec_io_write_fs(pid, tokens_instr, peticiones_memoria);
                    }
                    else{
                        exec_io_read_fs(pid, tokens_instr, peticiones_memoria);
                    }
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

static void exec_io_write_fs(int pid, char** tokens_instr, t_list *peticiones_memoria){   
    char* nombre_archivo = tokens_instr[2];
    char* puntero_archivo = tokens_instr[4];
    size_t tam_total = peticiones_tam_total(peticiones_memoria) + 1;

    log_info(logger, "PID: <%d> - Escribir Archivo: <%s> - Tamaño a Leer: <%ld> - Puntero Archivo: <%s>", 
    pid, nombre_archivo, tam_total, puntero_archivo);

    void* datos = malloc(tam_total);
    memset(datos, 0, tam_total);
    void* ptr_string = datos;

    for (int i = 0; i < list_size(peticiones_memoria); i++){
            t_peticion_memoria* peticion = list_get(peticiones_memoria, i);
            log_info(logger, "Peticion [%d] de [%d]", i+1, list_size(peticiones_memoria));
            peticion_lectura_enviar(peticion, &ptr_string, memory_socket);
            controlar_peticion();
            log_info(logger, "Peticion [%d] Recibido: [%s]", i+1, (char*)datos);
    }

    log_info(logger, "Leido desde memoria: [%s]", (char*) datos);

    

    if(!escribir_archivo(nombre_archivo, datos, tam_total, atoi(puntero_archivo))){
        log_error(logger, "No se pudo escribir el archivo");
        // TODO: Enviar respuesta de error?
    }

    log_info(logger, "Se ha escrito correctamente el archivo: [%s]", nombre_archivo);

    enviar_status(FIN_EJECUCION_IO, kernel_socket);  
}

static void exec_io_read_fs(int pid, char** tokens_instr, t_list *peticiones_memoria){
    char* nombre_archivo = tokens_instr[2];
    char* puntero_archivo = tokens_instr[4];
    size_t tam_total = peticiones_tam_total(peticiones_memoria) + 1;

    log_info(logger, "PID: <%d> - Leer Archivo: <%s> - Tamaño a Leer: <%ld> - Puntero Archivo: <%s>", 
    pid, nombre_archivo, tam_total, puntero_archivo);
    
    char* datos_leidos = malloc(tam_total);
    if (datos_leidos == NULL) {
        log_error(logger, "Error al asignar memoria para datos_leidos");
        return; // TODO: Enviar respuesta de error?
    }

    memset(datos_leidos, 0, tam_total);

    if(!leer_archivo(nombre_archivo,atoi(puntero_archivo), tam_total, (void*)&datos_leidos)){
        log_error(logger, "No se pudo leer el archivo");
        // TODO: Enviar respuesta de error?
    }

    log_info(logger, "Leido desde archivo: [%s]", datos_leidos);

    // Distribuir dato entre todas las peticiones
    peticiones_distribuir_dato(peticiones_memoria, datos_leidos, tam_total-1); // TODO ???

    for (int i = 0; i < list_size(peticiones_memoria); i++){
        t_peticion_memoria* peticion = list_get(peticiones_memoria, i);
        peticion_escritura_enviar(peticion, memory_socket);
        controlar_peticion();
    }
    
    // devolver status
    enviar_status(FIN_EJECUCION_IO, kernel_socket);
    log_info(logger, "Instruccion IO ejecutada");
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

