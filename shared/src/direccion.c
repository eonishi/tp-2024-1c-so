#include "../include/direccion.h"

t_peticion_memoria *peticion_crear(uint32_t direccion_fisica, void *dato, size_t tam_dato){
    t_peticion_memoria *direccion = malloc(sizeof(t_peticion_memoria));
    direccion->direccion_fisica = direccion_fisica;
    direccion->dato = dato;
    direccion->tam_dato = tam_dato;
    return direccion;
}

void peticion_destruir(t_peticion_memoria *direccion){
    free(direccion->dato);
    free(direccion);
}

void peticion_enviar(t_peticion_memoria *peticion_a_enviar, op_code CODE, int socket){
    t_paquete* paquete = crear_paquete(CODE);

    peticion_empaquetar(peticion_a_enviar, paquete, CODE);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

t_peticion_memoria* peticion_recibir(int socket, op_code CODE){
    t_list* paquete_recibido = recibir_paquete(socket);
    t_peticion_memoria *peticion = malloc(sizeof(t_peticion_memoria));

    peticion->direccion_fisica = deserializar_uint32(list_get(paquete_recibido, 0));
    peticion->tam_dato = deserializar_uint32(list_get(paquete_recibido, 1));
    if(CODE == ESCRIBIR_DATO_EN_MEMORIA){
        peticion->dato = list_get(paquete_recibido, 2);
    }

    list_destroy(paquete_recibido);
    return peticion;
}

// El codigo de operación es para saber si se va a escribir o leer en memoria
// y en funcion de eso se asigna memoria al dato.
void peticion_empaquetar(t_peticion_memoria *peticion, t_paquete *paquete, op_code CODE){
    // Se usa un paquete para enviar una peticion a memoria. Cada posicion de la lista es un dato de la peticion (recibir_paquete)
    void* df_stream = serializar_uint32(peticion->direccion_fisica);
    agregar_a_paquete(paquete, df_stream, sizeof(uint32_t));

    void* tam_dato_stream = serializar_uint32(peticion->tam_dato);
    agregar_a_paquete(paquete, tam_dato_stream, sizeof(uint32_t));

    if(CODE == ESCRIBIR_DATO_EN_MEMORIA){
        void *dato_stream = malloc(peticion->tam_dato);
        memcpy(dato_stream, peticion->dato, peticion->tam_dato);
        agregar_a_paquete(paquete, dato_stream, peticion->tam_dato);
    }

    free(df_stream);
    free(tam_dato_stream);
    //free(dato_stream); //Este free es condicional, depende de si se envia o no el dato
}

void* serializar_peticion_memoria(t_peticion_memoria* peticion){
    // cantidad_bytes = [direccion_fisica + tam_dato]
        void* stream = malloc(sizeof(uint32_t)*2);
        size_t offset = 0;

        memcpy(stream + offset, &peticion->direccion_fisica, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        memcpy(stream + offset, &peticion->tam_dato, sizeof(uint32_t));
        return stream;
}

void* deserializar_peticion_memoria(void* stream){
    t_peticion_memoria* peticion = malloc(sizeof(t_peticion_memoria));
    size_t offset = 0;

    memcpy(&peticion->direccion_fisica, stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(&peticion->tam_dato, stream + offset, sizeof(uint32_t));

    free(stream); 
    return peticion;
}

void peticiones_empaquetar(t_list* peticiones, t_paquete* paquete){
    // Cada petición queda en una posicion de la lista (recibir_paquete)
    size_t cantidad_peticiones = list_size(peticiones);
    log_peticiones(peticiones);
    agregar_a_paquete(paquete, &cantidad_peticiones, sizeof(size_t));
    log_info(logger, "Cantidad de peticiones: [%d]", cantidad_peticiones);

    for(int i = 0; i < cantidad_peticiones; i++){
        t_peticion_memoria* peticion = list_get(peticiones, i);
        log_peticion(peticion);
        void* stream = serializar_peticion_memoria(peticion);
        agregar_a_paquete(paquete, stream, sizeof(t_peticion_memoria));
    }
}

t_list* peticiones_desempaquetar(t_list* paquete_lista){
    // Solo sirve para paquetes que contengan SOLO peticiones

    // Como ya estan en una lista, pero serializadas,
    // se aplicar el deserializador a cada uno dentro de la misma lista.
    list_map(paquete_lista, deserializar_peticion_memoria); //🫥 a checkear si anda esto
}

t_list* peticiones_desempaquetar_segun_index(t_list* paquete_lista, int lista_index){
    size_t cantidad_peticiones = deserializar_uint32(list_get(paquete_lista, lista_index));
    log_info(logger, "Cantidad de peticiones: [%d]", cantidad_peticiones);
    lista_index++;

    t_list *peticiones = list_create();

    for (size_t i = 0; i < cantidad_peticiones; i++){
        void* peticion_bytes = list_get(paquete_lista, lista_index + i);
        t_peticion_memoria* peticion = deserializar_peticion_memoria(peticion_bytes);
        list_add(peticiones, peticion);
    }
    
    return peticiones;
}

size_t peticiones_tam_total(t_list* peticiones){
    size_t acumulador = 0;
    for(int i = 0; i < list_size(peticiones); i++){
        t_peticion_memoria* peticion = list_get(peticiones, i);
        acumulador += peticion->tam_dato;
    }
    return acumulador;
}

void peticiones_distribuir_dato(t_list* peticiones, void* dato_entero, size_t tam_dato){
    if(peticiones_tam_total(peticiones) != tam_dato) return;

    void* ptr_dato_entero = dato_entero; 

    size_t offset = 0;
    for(int i = 0; i < list_size(peticiones); i++){
        t_peticion_memoria* peticion = list_get(peticiones, i);
        peticion->dato = malloc(peticion->tam_dato);
        memcpy(peticion->dato, ptr_dato_entero + offset, peticion->tam_dato);
        offset += peticion->tam_dato;
    }
}

void peticion_escritura_enviar(t_peticion_memoria* peticion_a_enviar, int socket_memoria){
    log_info(logger, "Enviando solicitud de escritura a memoria. Dirección: [%d], Tam_Dato: [%d]", peticion_a_enviar->direccion_fisica, peticion_a_enviar->tam_dato);
    peticion_enviar(peticion_a_enviar, ESCRIBIR_DATO_EN_MEMORIA, socket_memoria);
}

void peticion_lectura_enviar(t_peticion_memoria* peticion_a_enviar, void** ptr_donde_se_guarda_dato, int socket_memoria){
    log_info(logger, "Enviando solicitud de leer a memoria. Dirección: [%d], Tam_Dato: [%d]", peticion_a_enviar->direccion_fisica, peticion_a_enviar->tam_dato);
    peticion_enviar(peticion_a_enviar, LEER_DATO_DE_MEMORIA, socket_memoria);

    int size;
    void *parte_del_dato = recibir_buffer(&size, socket_memoria);
    memcpy(*ptr_donde_se_guarda_dato, parte_del_dato, size);
    *ptr_donde_se_guarda_dato += size;
    free(parte_del_dato);
}

void log_peticion(void* peticion_bytes){
    t_peticion_memoria* peticion = (t_peticion_memoria*) peticion_bytes;
    log_info(logger, "Dirección: [%d], Tam_Dato: [%d], Dato: [%d]", peticion->direccion_fisica, peticion->tam_dato, peticion->dato);
}

void log_peticiones(t_list* peticiones){
    list_iterate(peticiones, log_peticion);
}