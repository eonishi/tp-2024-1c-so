#include "../include/protocolo.h"


void* serializar_int8(int8_t value){
    void* stream = malloc(sizeof(int8_t)); 

  // Datos de los registros
    memcpy(stream, &value, sizeof(int8_t));
    
    return stream;
}

int8_t deserializar_int8(void* int_bytes){
    int8_t* result =malloc(sizeof(int8_t));
    int offset = 0;

    // Datos con tipo de dato primitivo dentro del PCB
    memcpy(result, int_bytes, sizeof(int8_t));

    return *result;
}

void* serializar_char(char* string){
    size_t size = strlen(string) + 1;
    void* stream = malloc(size); 

  // Datos de los registros
    memcpy(stream, string, size);
    
    return stream;
}

char* deserializar_char(void* char_bytes, int8_t size){
    char* string = (char*) malloc(size);
    int offset = 0;

    // Datos con tipo de dato primitivo dentro del PCB
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
    free(paquete->buffer);
    free(paquete);
    
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