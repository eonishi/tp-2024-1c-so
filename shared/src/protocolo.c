#include "../include/protocolo.h"

// Kernel - Memoria
int enviar_solicitud_crear_proceso(char* filePath, unsigned PID, int socket_cliente){
    t_paquete* paquete = crear_paquete(CREAR_PROCESO_EN_MEMORIA);

    void* tamanio_path_serializado = serializar_size_t(strlen(filePath) + 1);
    agregar_a_paquete(paquete, tamanio_path_serializado, sizeof(size_t));

    void* filepath_serializado = serializar_char(filePath);
    agregar_a_paquete(paquete, filepath_serializado, strlen(filePath) + 1);

    void* pid_stream = serializar_unsigned(PID);    
    agregar_a_paquete(paquete, pid_stream, sizeof(unsigned));

    enviar_paquete(paquete, socket_cliente);

    free(pid_stream);
    free(filepath_serializado);
    free(tamanio_path_serializado);
    eliminar_paquete(paquete);
}

solicitud_crear_proceso recibir_solicitud_crear_proceso(int socket_cliente){
    log_info(logger,"Recibido en solicitud crear proceso..");
    t_list* lista_pcb_bytes = recibir_paquete(socket_cliente);

    void* filepath_tamanio_bytes= list_get(lista_pcb_bytes, 0);
    void* filepath_bytes= list_get(lista_pcb_bytes, 1);
    void* pid_bytes = list_get(lista_pcb_bytes, 2);


    size_t tamanio_filepath = deserializar_size_t(filepath_tamanio_bytes);
    log_info(logger,"tamanio_filepath recibido en solicitud crear proceso: [%d]", tamanio_filepath);
    char* file_path = deserializar_char(filepath_bytes,tamanio_filepath);
    log_info(logger,"FilePath recibido en solicitud crear proceso: [%s]", file_path);

    unsigned pid_recibido = deserializar_unsigned(pid_bytes);

    free(lista_pcb_bytes);

    solicitud_crear_proceso respuesta;
    respuesta.filePath = file_path;
    respuesta.PID = pid_recibido;

    return respuesta;
}

solicitud_conexion_kernel* crear_solicitud_conexion_kernel(char* nombre_interfaz, io_tipo tipo, int* operaciones){
    solicitud_conexion_kernel* solicitud = malloc(sizeof(solicitud_conexion_kernel));
    solicitud->nombre_interfaz = nombre_interfaz;
    solicitud->tipo = tipo;
    solicitud->operaciones = operaciones;

    return solicitud;
}

void liberar_solicitud_conexion_kernel(solicitud_conexion_kernel* solicitud){
    free(solicitud->operaciones);
    free(solicitud);
}

void enviar_solicitud_conexion_kernel(solicitud_conexion_kernel solicitud, int kernel_skt){
    size_t size_nombre = strlen(solicitud.nombre_interfaz) + 1;
    t_paquete* paquete = crear_paquete(HANDSHAKE);

    void* stream_nombre_interfaz = serializar_char(solicitud.nombre_interfaz);
    agregar_a_paquete(paquete, stream_nombre_interfaz, size_nombre);

    void* stream_tipo = serializar_int(solicitud.tipo);
    agregar_a_paquete(paquete, stream_tipo, sizeof(int));

    size_t size_op = size_io_op(solicitud.tipo);
    void* stream_op = serializar_io_operaciones(solicitud.operaciones, size_op);
    agregar_a_paquete(paquete, stream_op, size_op);

    enviar_paquete(paquete, kernel_skt);
    log_info(logger, "Solicitud de [%s] conexion enviada", solicitud.nombre_interfaz);

    free(stream_nombre_interfaz);
    free(stream_tipo);
    free(stream_op);
    eliminar_paquete(paquete);
}

solicitud_conexion_kernel* recibir_solicitud_conexion_kernel(int socket_de_una_io){
    t_list* lista_bytes = recibir_paquete(socket_de_una_io);

    char* nombre_interfaz = list_get(lista_bytes, 0);
    void* tipo = list_get(lista_bytes, 1);
    void* instrucciones = list_get(lista_bytes, 2);

    io_tipo tipo_enum = deserializar_int(tipo);
    int* op_disponibles = deserializar_io_operaciones(instrucciones, tipo_enum);

    free(lista_bytes);
    free(tipo);
    free(instrucciones);

    solicitud_conexion_kernel* solicitud = crear_solicitud_conexion_kernel(nombre_interfaz, tipo_enum, op_disponibles);
    return solicitud;
}


int enviar_instruccion_io(char** instruccion_tokenizada, t_list* peticiones_memoria, int pid, int socket_cliente){
    // TODO: teniendo en cuenta que la validación de la existencia de io y correspondencia de instruccion
    // la hace kernel, es redundante enviar toda la instruccion tokenizada. Se podría enviar lo necesario para la ejecucion.

    t_paquete* paquete = crear_paquete(EJECUTAR_INSTRUCCION_IO);

    void* stream_pid = serializar_int(pid);
    agregar_a_paquete(paquete, stream_pid, sizeof(int));

    serializar_lista_strings_y_agregar_a_paquete(instruccion_tokenizada, paquete);

    log_info(logger, "Estoy por empaquetar peticiones de memoria");
    peticiones_empaquetar(peticiones_memoria, paquete);
    log_info(logger, "Estoy por empaquetar peticiones de memoria");

    enviar_paquete(paquete, socket_cliente);
    eliminar_paquete(paquete);
}

// Solucion temporal: recibo un puntero a una lista de peticiones de memoria
solicitud_instruccion_io recibir_instruccion_io(int socket_cliente){
    t_list* bytes = recibir_paquete(socket_cliente);

    int pid = deserializar_int(list_get(bytes, 0));

    char** tokens = deserializar_lista_strings(bytes, 1);

    t_list* peticiones_memoria = peticiones_desempaquetar_segun_index(bytes, 2 + string_array_size(tokens));

    solicitud_instruccion_io result = {pid, tokens, peticiones_memoria};

    log_warning(logger, "PID:[%d] tokens[0]:[%s] ", pid, tokens[0]);

    return result;
}

void enviar_solicitud_truncar_archivo_fs(solicitud_truncar_archivo solicitud, int socket){
    size_t size_nombre = strlen(solicitud.nombre_archivo) + 1;
    t_paquete* paquete = crear_paquete(TRUNCAR_ARCHIVO_FS);

    void* stream_nombre_archivo = serializar_char(solicitud.nombre_archivo);
    agregar_a_paquete(paquete, stream_nombre_archivo, size_nombre);

    void* stream_tamanio_archivo = serializar_int(solicitud.tamanio_archivo);
    agregar_a_paquete(paquete, stream_tamanio_archivo, sizeof(int));

    void* stream_pid = serializar_int(solicitud.pid);
    agregar_a_paquete(paquete, stream_pid, sizeof(int));

    enviar_paquete(paquete, socket);

    free(stream_nombre_archivo);
    free(stream_tamanio_archivo);
    free(stream_pid);

    eliminar_paquete(paquete);
}

solicitud_truncar_archivo recibir_solicitud_truncar_archivo_fs(int socket){
    t_list* lista_bytes = recibir_paquete(socket);

    char* nombre_archivo = list_get(lista_bytes, 0);
    void* tam_archivo_bytes = list_get(lista_bytes, 1);
    void* pid_bytes = list_get(lista_bytes, 2);

    int tam_archivo = deserializar_int(tam_archivo_bytes);
    int pid = deserializar_int(pid_bytes);

    free(lista_bytes);
    free(tam_archivo_bytes);
    free(pid_bytes);

    solicitud_truncar_archivo solicitud = {nombre_archivo, tam_archivo,pid};

    return solicitud;
}


void enviar_solicitud_accion_archivo_fs(int accion,char* nombre, int pid, int socket){
    size_t size_nombre = strlen(nombre) + 1;
    t_paquete* paquete = crear_paquete(accion);

    void* stream_nombre_archivo = serializar_char(nombre);
    agregar_a_paquete(paquete, stream_nombre_archivo, size_nombre);

    void* stream_pid = serializar_int(pid);
    agregar_a_paquete(paquete, stream_pid, sizeof(int));

    enviar_paquete(paquete, socket);

    free(stream_nombre_archivo);
    free(stream_pid);

    eliminar_paquete(paquete);
}

solicitud_accion_archivo recibir_solicitud_accion_archivo_fs(int socket){
    t_list* lista_bytes = recibir_paquete(socket);

    char* nombre_archivo = list_get(lista_bytes, 0);
    void* pid_bytes = list_get(lista_bytes, 1);

    int pid = deserializar_int(pid_bytes);

    free(lista_bytes);
    free(pid_bytes);

    solicitud_accion_archivo solicitud = {nombre_archivo,pid};

    return solicitud;
}


void enviar_io_sleep(int retraso, int pid, int socket){
    t_paquete* paquete = crear_paquete(EJECUTAR_INSTRUCCION_IO);

    void* stream_retraso = serializar_int(retraso);
    agregar_a_paquete(paquete, stream_retraso, sizeof(int));

    void* stream_pid = serializar_int(pid);
    agregar_a_paquete(paquete, stream_pid, sizeof(int));

    enviar_paquete(paquete, socket);

    free(stream_retraso);
    free(stream_pid);

    eliminar_paquete(paquete);
}

solicitud_io_sleep recibir_io_sleep(int socket){
    t_list* lista_bytes = recibir_paquete(socket);

    void* retraso_bytes = list_get(lista_bytes, 0);
    void* pid_bytes = list_get(lista_bytes, 1);

    int retraso = deserializar_int(retraso_bytes);
    int pid = deserializar_int(pid_bytes);

    free(lista_bytes);
    free(retraso_bytes);
    free(pid_bytes);

    solicitud_io_sleep solicitud = {retraso, pid};

    return solicitud;
}