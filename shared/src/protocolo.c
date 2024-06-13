#include "../include/protocolo.h"

// Usar siempre como última serializacion. Ejemplo: {pcb: "", lista_string: []}
void serializar_lista_strings_y_agregar_a_paquete(char** lista_strings, t_paquete* paquete){

    void* cantidad_tokens = serializar_int(string_array_size(lista_strings));

    agregar_a_paquete(paquete, cantidad_tokens, sizeof(int));

    for(int index = 0 ; index < string_array_size(lista_strings); index++){
        void* token = serializar_char(lista_strings[index]);

        agregar_a_paquete(paquete, token, strlen(lista_strings[index]) + 1);
    }
}

char** deserializar_lista_strings(t_list* bytes, int index_cantidad_tokens){ 
    int cantidad_tokens = deserializar_int(list_get(bytes, index_cantidad_tokens)); 

    char** tokens = string_array_new();
    
    for(int i = index_cantidad_tokens+1;i < cantidad_tokens+index_cantidad_tokens+1; i++){
        char* token = list_get(bytes, i);

        string_array_push(&tokens, token);
    }    

    return tokens;
}


void* serializar_int8(int8_t value){
    void* stream = malloc(sizeof(int8_t)); 

    memcpy(stream, &value, sizeof(int8_t));
    
    return stream;
}

void* serializar_int(int value){
    void* stream = malloc(sizeof(int)); 

    memcpy(stream, &value, sizeof(int));
    
    return stream;
}

int8_t deserializar_int8(void* int_bytes){
    int8_t* result =malloc(sizeof(int8_t));
    int offset = 0;

    memcpy(result, int_bytes, sizeof(int8_t));

    return *result;
}

int deserializar_int(void* int_bytes){
    int* result =malloc(sizeof(int));
    memcpy(result, int_bytes, sizeof(int));

    return *result;
}


void* serializar_char(char* string){
    size_t size = strlen(string) + 1;
    void* stream = malloc(size); 

    memcpy(stream, string, size);
    
    return stream;
}

char* deserializar_char(void* char_bytes, int8_t size){
    char* string = (char*) malloc(size);
    int offset = 0;

    memcpy(string, char_bytes, size);

    return string;
}

// Kernel - Memoria
int enviar_solicitud_crear_proceso(char* filePath, pcb* pcb, int socket_cliente){
    t_paquete* paquete = crear_paquete(CREAR_PROCESO_EN_MEMORIA);

    void* tamanio_path_serializado = serializar_int8(strlen(filePath) + 1);
    agregar_a_paquete(paquete, tamanio_path_serializado, sizeof(int8_t));

    void* filepath_serializado = serializar_char(filePath);
    agregar_a_paquete(paquete, filepath_serializado, strlen(filePath) + 1);

    void* pcb_serializado = serializar_pcb(pcb);    
    agregar_a_paquete(paquete, pcb_serializado, pcb_size());

    enviar_paquete(paquete, socket_cliente);

    free(pcb_serializado);
    free(filepath_serializado);
    free(tamanio_path_serializado);
    eliminar_paquete(paquete);
}

solicitud_crear_proceso recibir_solicitud_crear_proceso(int socket_cliente){
    log_info(logger,"Recibido en solicitud crear proceso..");
    t_list* lista_pcb_bytes = recibir_paquete(socket_cliente);

    void* filepath_tamanio_bytes= list_get(lista_pcb_bytes, 0);
    void* filepath_bytes= list_get(lista_pcb_bytes, 1);
    void* pcb_bytes = list_get(lista_pcb_bytes, 2);


    int8_t tamanio_filepath = deserializar_int8(filepath_tamanio_bytes);
    log_info(logger,"tamanio_filepath recibido en solicitud crear proceso: [%d]", tamanio_filepath);
    char* file_path = deserializar_char(filepath_bytes,tamanio_filepath);
    log_info(logger,"FilePath recibido en solicitud crear proceso: [%s]", file_path);

    pcb* pcb = deserializar_pcb_new(pcb_bytes);

    free(lista_pcb_bytes);

    solicitud_crear_proceso respuesta;
    respuesta.filePath = file_path;
    respuesta.pcb = pcb;

    return respuesta;
}

size_t size_io_op(io_tipo tipo){
    return (tipo == DIALFS ? sizeof(int)*5 : sizeof(int));
}

void* serializar_io_operaciones(int* instrucciones, size_t size){
    void* stream = malloc(size);
    memcpy(stream, instrucciones, size);
    return stream;
}
int* deserializar_io_operaciones(void* instr_bytes, io_tipo tipo){
    size_t size = size_io_op(tipo);
    int* instrucciones = malloc(size);
    memcpy(instrucciones, instr_bytes, size);
    return instrucciones;
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


int enviar_bloqueo_por_io(solicitud_bloqueo_por_io solicitud, int socket_cliente){
    t_paquete* paquete = crear_paquete(PROCESO_BLOQUEADO);

    void* pcb_serializado = serializar_pcb(solicitud.pcb);    

    agregar_a_paquete(paquete, pcb_serializado, pcb_size());

    serializar_lista_strings_y_agregar_a_paquete(solicitud.instruc_io_tokenizadas, paquete);

    enviar_paquete(paquete, socket_cliente);
}

solicitud_bloqueo_por_io recibir_solicitud_bloqueo_por_io(int socket_cliente){
    log_info(logger,"Recibido en solicitud bloqueo por io");

    t_list* bytes = recibir_paquete(socket_cliente);
    
    void* pcb_bytes = list_get(bytes, 0);
    pcb* pcb = deserializar_pcb_new(pcb_bytes);

    char** tokens = deserializar_lista_strings(bytes, 1);

    solicitud_bloqueo_por_io solicitud;

    solicitud.pcb = pcb;
    solicitud.instruc_io_tokenizadas = tokens;

    return solicitud;
}


int enviar_instruccion_io(char** instruccion_tokenizada, int socket_cliente){
    t_paquete* paquete = crear_paquete(EJECUTAR_INSTRUCCION_IO);

    serializar_lista_strings_y_agregar_a_paquete(instruccion_tokenizada, paquete);

    enviar_paquete(paquete, socket_cliente);
}

char** recibir_instruccion_io(int socket_cliente){
    log_info(logger,"Recibido en solicitud instruccion io");
    t_list* bytes = recibir_paquete(socket_cliente);

    log_info(logger,"Paquete recibido");

    char** tokens = deserializar_lista_strings(bytes, 0);

    return tokens;
}


void* serializar_uint32(uint32_t value){
    void* stream = malloc(sizeof(uint32_t)); 

    memcpy(stream, &value, sizeof(uint32_t));
    
    return stream;
}

uint32_t deserializar_uint32(void* int_bytes){
    uint32_t* result =malloc(sizeof(uint32_t));
    memcpy(result, int_bytes, sizeof(uint32_t));

    return *result;
}


int enviar_escribir_dato_en_memoria(uint32_t direccion, uint32_t dato, int socket_cliente){
    log_info(logger, "Enviando escritura a memoria. Dirección: [%d], Dato:[%d]", direccion, dato);

    t_paquete* paquete = crear_paquete(ESCRIBIR_DATO_EN_MEMORIA);

    void* direccion_serializada = serializar_uint32(direccion);
    agregar_a_paquete(paquete, direccion_serializada, sizeof(uint32_t));

    void* dato_serializado = serializar_uint32(dato);
    agregar_a_paquete(paquete, dato_serializado, sizeof(uint32_t));

    enviar_paquete(paquete, socket_cliente);
}

solicitud_escribir_dato_en_memoria recibir_escribir_dato_en_memoria(int socket_cliente){
    t_list* lista_bytes = recibir_paquete(socket_cliente);

    solicitud_escribir_dato_en_memoria respuesta;

    respuesta.direccion = deserializar_uint32(list_get(lista_bytes, 0));
    respuesta.dato = deserializar_uint32(list_get(lista_bytes, 1));

    return respuesta;
}


int enviar_solicitud_leer_dato_de_memoria(uint32_t direccion, int socket_cliente){
    log_info(logger, "Enviando escritura a memoria. Dirección: [%d]", direccion);

    t_paquete* paquete = crear_paquete(LEER_DATO_DE_MEMORIA);

    void* direccion_serializada = serializar_uint32(direccion);
    agregar_a_paquete(paquete, direccion_serializada, sizeof(uint32_t));

    enviar_paquete(paquete, socket_cliente);
}

uint32_t recibir_solicitud_leer_dato_de_memoria(int socket_cliente){
     t_list* lista_bytes = recibir_paquete(socket_cliente);

     return deserializar_uint32(list_get(lista_bytes, 0));
}


void enviar_dato_leido_de_memoria(uint32_t dato, int socket_cliente){
    log_info(logger, "Enviando dato leido de memoria. Dato: [%d]", dato);

    t_paquete* paquete = crear_paquete(DATO_LEIDO_DE_MEMORIA);

    void* direccion_serializada = serializar_uint32(dato);
    agregar_a_paquete(paquete, direccion_serializada, sizeof(uint32_t));

    enviar_paquete(paquete, socket_cliente);
}

uint32_t recibir_dato_leido_de_memoria(int socket){
     t_list* lista_bytes = recibir_paquete(socket);

     return deserializar_uint32(list_get(lista_bytes, 0));
}