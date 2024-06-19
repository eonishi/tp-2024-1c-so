#include "../include/mmu.h"

unsigned TAM_PAGINA;

unsigned obtener_numero_pagina(unsigned direccion_logica){
    return floor(direccion_logica / TAM_PAGINA);
}

unsigned obtener_offset(unsigned direccion_logica, unsigned numero_pagina){
    return direccion_logica - (numero_pagina * TAM_PAGINA);
}

unsigned cantidad_paginas_a_consultar(unsigned offset, size_t tam_dato){
    return (offset + tam_dato + TAM_PAGINA - 1) / TAM_PAGINA;
}

unsigned consultar_tabla_de_paginas(unsigned numero_pagina){
    enviar_cantidad(numero_pagina, CONSULTAR_TABLA_DE_PAGINAS, socket_memoria);

    if(!recibir_operacion(socket_memoria) == CONSULTAR_TABLA_DE_PAGINAS){
        log_error(logger, "Error al consultar la tabla de paginas");
        return ERROR;
    }

    unsigned numero_frame_consultado = recibir_cantidad(socket_memoria);
    return numero_frame_consultado;
}

unsigned min(unsigned a, unsigned b){
    return (a < b)? a : b;
}

t_list* mmu(unsigned direccion_logica, size_t tam_dato, void* dato){
    unsigned numero_pagina_inicial = obtener_numero_pagina(direccion_logica);
    unsigned offset = obtener_offset(direccion_logica, numero_pagina_inicial);
    unsigned paginas_a_consultar = cantidad_paginas_a_consultar(offset, tam_dato);
    log_info(logger, "Cantidad de paginas a consultar: [%d]", paginas_a_consultar);

    t_list* direcciones_fisica = list_create(); 
    void* ptr_byte_dato = dato;
    for (size_t i = 0; i < paginas_a_consultar; i++){
        size_t bytes_a_leer;
        unsigned numero_pagina = numero_pagina_inicial + i;
        // Existe en la TLB?
        if(tlb_tiene_entrada(pcb_actual->pid, numero_pagina)){
            log_info(logger, "HIT TLB!!! i: [%d], numero_pagina:[%d], ", i, numero_pagina);
            if(i==0){
                unsigned frame_number = obtener_frame_tlb(pcb_actual->pid, numero_pagina);
                bytes_a_leer = min(tam_dato, TAM_PAGINA - offset);
                t_peticion_memoria* nueva_peticion = peticion_crear(frame_number * TAM_PAGINA+offset, ptr_byte_dato, bytes_a_leer);
                list_add(direcciones_fisica, nueva_peticion);
            }
            else{
                unsigned frame_number = obtener_frame_tlb(pcb_actual->pid, numero_pagina);
                t_peticion_memoria* nueva_peticion = peticion_crear(frame_number * TAM_PAGINA, ptr_byte_dato, bytes_a_leer);
                list_add(direcciones_fisica, nueva_peticion);
            }
        }

        // No existe en la TLB, Hacer consulta a la tabla de paginas:
        else{
            log_info(logger, "MISS TLB!!! i: [%d], numero_pagina:[%d], ", i, numero_pagina);
            unsigned frame_number = consultar_tabla_de_paginas(numero_pagina);
            agregar_entrada_tlb(pcb_actual->pid, numero_pagina, frame_number);
            if(i == 0){
                bytes_a_leer = min(tam_dato, TAM_PAGINA - offset);
                t_peticion_memoria* nueva_peticion = peticion_crear(frame_number * TAM_PAGINA+offset, ptr_byte_dato, bytes_a_leer);
                list_add(direcciones_fisica, nueva_peticion);
            }
            else{
                bytes_a_leer = min(tam_dato, TAM_PAGINA);
                t_peticion_memoria* nueva_peticion = peticion_crear(frame_number * TAM_PAGINA, ptr_byte_dato, bytes_a_leer);
                list_add(direcciones_fisica, nueva_peticion);
            }
        }

        tam_dato -= bytes_a_leer;
        ptr_byte_dato += bytes_a_leer;
    }

    return direcciones_fisica;
}