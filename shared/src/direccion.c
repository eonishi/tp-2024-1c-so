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

    void* df_stream = serializar_uint32(peticion_a_enviar->direccion_fisica);
    agregar_a_paquete(paquete, df_stream, sizeof(uint32_t));

    void* tam_dato_stream = serializar_uint32(peticion_a_enviar->tam_dato);
    agregar_a_paquete(paquete, tam_dato_stream, sizeof(uint32_t));

    if(CODE == ESCRIBIR_DATO_EN_MEMORIA){
        void *dato_stream = malloc(peticion_a_enviar->tam_dato);
        memcpy(dato_stream, peticion_a_enviar->dato, peticion_a_enviar->tam_dato);
        agregar_a_paquete(paquete, dato_stream, peticion_a_enviar->tam_dato);
    }

    enviar_paquete(paquete, socket);
    //free(dato_stream); //Este free es condicional, depende de si se envia o no el dato
    free(df_stream);
    free(tam_dato_stream);
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