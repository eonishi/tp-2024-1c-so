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