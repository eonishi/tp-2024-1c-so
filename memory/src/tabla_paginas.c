#include "../include/tabla_paginas.h"

static t_tabla_paginas* get_table_by_PID(){
    if(!list_any_satisfy(procesos_en_memoria, memoria_tiene_pid_solicitado)){
        log_error(logger, "No se encontró el PID solicitado");
        abort();
    }

    t_proceso_en_memoria* proceso_buscado = list_find(procesos_en_memoria, memoria_tiene_pid_solicitado);
    return proceso_buscado->tabla_paginas;
}

void agregar_paginas(unsigned cantidad_de_paginas){
    t_tabla_paginas* tabla = get_table_by_PID();
    for(size_t i = 0; i < cantidad_de_paginas; i++){
        unsigned frame_number = get_available_frame();
        list_add(tabla, (void*)frame_number); 
        marcar_frame_como(frame_number, 1); // 1 = OCUPADO
    }
}

void quitar_paginas(size_t cantidad_de_paginas){
    t_tabla_paginas* tabla = get_table_by_PID();
    log_info(logger, "Cantidad de paginas a quitar: [%d]", cantidad_de_paginas);

    for (size_t i = 0; i < cantidad_de_paginas; i++){
        size_t index_tabla = list_size(tabla)-1;
        log_info(logger, "Index de la tabla: [%d]", index_tabla);

        unsigned frame_number = (unsigned)list_remove(tabla, index_tabla);
        log_info(logger, "Frame a liberar: [%d]", frame_number);

        log_info(logger, "Voy a ejecutar marcar_frame_como ");
        marcar_frame_como(frame_number, 0); // 0 = LIBRE
        log_info(logger, "Frame [%d] marcado como libre", frame_number);

        // free(frame_number);
        // Hay que liberarel numero de frame pero no puedo con el free 🤷🏻‍♂️
        log_info(logger, "Iteracion numero [%d]", i);
    }
};

unsigned get_frame_number_by_pagina(unsigned nro_pagina){
    t_tabla_paginas* tabla = get_table_by_PID();
    unsigned frame_number = (unsigned)list_get(tabla, nro_pagina);
    return frame_number;
}

size_t cantidad_de_paginas(){
    t_tabla_paginas* tabla = get_table_by_PID();
    return list_size(tabla);
}

void redimensionar_memoria_proceso(size_t cantidad_paginas){
    size_t cantidad_actual_de_paginas = cantidad_de_paginas();
    if(cantidad_paginas >= cantidad_actual_de_paginas){
        log_info(logger, "Aumentando memoria del proceso a [%d] paginas totales", cantidad_paginas);
        agregar_paginas(cantidad_paginas - cantidad_actual_de_paginas);
    }
    else{
        log_info(logger, "Disminuyendo memoria del proceso a [%d] paginas totales", cantidad_paginas);
        quitar_paginas(cantidad_actual_de_paginas - cantidad_paginas);
    }
}

void mostrar_tabla_paginas(){
    size_t cantidad_total_de_paginas = cantidad_de_paginas();
    for (int i = 0; i < cantidad_total_de_paginas; i++){
			unsigned frame_guardado = get_frame_number_by_pagina(i);
			log_info(logger, "Frame guardado en la posicion [%d]: %d", i ,frame_guardado);
		}
}

bool puedo_agregar_o_disminuir(int cantidad_paginas){
    size_t cantidad_actual_de_paginas = cantidad_de_paginas();
    if(cantidad_paginas >= cantidad_actual_de_paginas){
        return frames_libres() >= (cantidad_paginas - cantidad_actual_de_paginas);
    }
    else{
        return cantidad_actual_de_paginas - cantidad_paginas >= 0;
    }
}

unsigned calcular_cantidad_de_paginas_por_bytes(unsigned bytes){
    return (bytes + config.tam_pagina - 1) / config.tam_pagina; // división entera con redondeo hacia arriba
}