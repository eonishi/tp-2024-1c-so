#include "../include/protocolo.h"

// Kernel - Memoria
int enviar_solicitud_crear_proceso(char* filePath, unsigned PID, int socket_cliente){
    t_paquete* paquete = crear_paquete(CREAR_PROCESO_EN_MEMORIA);

    void* tamanio_path_serializado = serializar_int8(strlen(filePath) + 1);
    agregar_a_paquete(paquete, tamanio_path_serializado, sizeof(int8_t));

    void* filepath_serializado = serializar_char(filePath);
    agregar_a_paquete(paquete, filepath_serializado, strlen(filePath) + 1);

    void* pid_stream = serializar_uint32(PID);    
    agregar_a_paquete(paquete, pid_stream, sizeof(uint32_t));

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


    int8_t tamanio_filepath = deserializar_int8(filepath_tamanio_bytes);
    log_info(logger,"tamanio_filepath recibido en solicitud crear proceso: [%d]", tamanio_filepath);
    char* file_path = deserializar_char(filepath_bytes,tamanio_filepath);
    log_info(logger,"FilePath recibido en solicitud crear proceso: [%s]", file_path);

    unsigned pid_recibido = deserializar_uint32(pid_bytes);

    free(lista_pcb_bytes);

    solicitud_crear_proceso respuesta;
    respuesta.filePath = file_path;
    respuesta.PID = pid_recibido;

    return respuesta;
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

solicitud_conexion_kernel recibir_solicitud_conexion_kernel(int socket_de_una_io){
    t_list* lista_bytes = recibir_paquete(socket_de_una_io);

    char* nombre_interfaz = list_get(lista_bytes, 0);
    void* tipo = list_get(lista_bytes, 1);
    void* instrucciones = list_get(lista_bytes, 2);

    io_tipo tipo_enum = deserializar_int(tipo);
    int* op_disponibles = deserializar_io_operaciones(instrucciones, tipo_enum);

    free(lista_bytes);
    free(tipo);
    free(instrucciones);

    solicitud_conexion_kernel solicitud = {nombre_interfaz, tipo_enum, op_disponibles};
    return solicitud;
}


int enviar_instruccion_io(char** instruccion_tokenizada, t_list* peticiones_memoria, int socket_cliente){
    // TODO: teniendo en cuenta que la validación de la existencia de io y correspondencia de instruccion
    // la hace kernel, es redundante enviar toda la instruccion tokenizada. Se podría enviar lo necesario para la ejecucion.

    t_paquete* paquete = crear_paquete(EJECUTAR_INSTRUCCION_IO);

    serializar_lista_strings_y_agregar_a_paquete(instruccion_tokenizada, paquete);

    log_info(logger, "Estoy por empaquetar peticiones de memoria");
    peticiones_empaquetar(peticiones_memoria, paquete);
    log_info(logger, "Estoy por empaquetar peticiones de memoria");

    enviar_paquete(paquete, socket_cliente);
    eliminar_paquete(paquete);
}

// Solucion temporal: recibo un puntero a una lista de peticiones de memoria
char** recibir_instruccion_io(int socket_cliente, t_list** peticiones_memoria){
    t_list* bytes = recibir_paquete(socket_cliente);

    char** tokens = deserializar_lista_strings(bytes, 0);

    *peticiones_memoria = peticiones_desempaquetar_segun_index(bytes, 1 + string_array_size(tokens));

    return tokens;
}