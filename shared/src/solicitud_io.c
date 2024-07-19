#include "../include/solicitud_io.h"

int enviar_bloqueo_por_io(solicitud_bloqueo_por_io solicitud, int socket_cliente){
    t_paquete* paquete = crear_paquete(PROCESO_BLOQUEADO);

    serializar_lista_strings_y_agregar_a_paquete(solicitud.instruc_io_tokenizadas, paquete);

    peticiones_empaquetar(solicitud.peticiones_memoria, paquete);

    enviar_paquete(paquete, socket_cliente);
}

solicitud_bloqueo_por_io recibir_solicitud_bloqueo_por_io(int socket_cliente){
    log_info(logger,"Recibido en solicitud bloqueo por io");

    t_list* paquete_bytes = recibir_paquete(socket_cliente);
    
    char** tokens = deserializar_lista_strings(paquete_bytes, 1);

    // Son dos porque la primera posicion es el pcb y la segunda es la cantidad de tokens 
    t_list* peticiones_memoria = peticiones_desempaquetar_segun_index(paquete_bytes, 2 + string_array_size(tokens));

    solicitud_bloqueo_por_io solicitud;
    solicitud.instruc_io_tokenizadas = tokens;
    solicitud.peticiones_memoria = peticiones_memoria;

    // free(pcb_bytes);
    return solicitud;
}

solicitud_bloqueo_por_io* crear_solicitud_io(char** instruc_io_tokenizadas, t_list* peticiones_memoria){
    solicitud_bloqueo_por_io* solicitud = malloc(sizeof(solicitud_bloqueo_por_io));
    solicitud->instruc_io_tokenizadas = instruc_io_tokenizadas;
    solicitud->peticiones_memoria = peticiones_memoria;

    return solicitud;
}

void liberar_solicitud_io(solicitud_bloqueo_por_io* solicitud){
    liberar_lista_strings(solicitud->instruc_io_tokenizadas);
    liberar_peticiones(solicitud->peticiones_memoria);
    free(solicitud);
}

void agregar_a_paquete_solicitud_io(solicitud_bloqueo_por_io* solicitud, t_paquete* paquete){
    // Checkeo si tengo que enviar instrucciones
    bool envio_instrucciones = solicitud->instruc_io_tokenizadas != NULL;
    log_warning(logger, "Enviando PCB con instrucciones? [%s]", envio_instrucciones? "true" : "false");

    if(envio_instrucciones){
        void* envio_una_solicitud = serializar_int(1);
        agregar_a_paquete(paquete, envio_una_solicitud, sizeof(int));

        serializar_lista_strings_y_agregar_a_paquete(solicitud->instruc_io_tokenizadas, paquete);
    }

    // Checkeo si tengo que enviar peticiones
    bool envio_peticiones = solicitud->peticiones_memoria != NULL;
    log_warning(logger, "Enviando PCB con peticiones? [%s]", envio_instrucciones? "true" : "false");

    if(envio_peticiones){
        void* envio_una_solicitud = serializar_int(1);
        agregar_a_paquete(paquete, envio_una_solicitud, sizeof(int));
        peticiones_empaquetar(solicitud->peticiones_memoria, paquete);
    }

    
    void* envio_una_solicitud = serializar_int(0);
    agregar_a_paquete(paquete, envio_una_solicitud, sizeof(int));
}