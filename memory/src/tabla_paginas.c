#include "../include/tabla_paginas.h"

t_dictionary *crear_tabla_paginas()
{
    return dictionary_create();
}

static t_tabla_paginas* get_table_by_PID(){
    if(!list_any_satisfy(procesos_en_memoria, memoria_tiene_pid_solicitado)){
        log_error(logger, "No se encontró el PID solicitado");
        abort();
    }

    t_proceso_en_memoria* proceso_buscado = list_find(procesos_en_memoria, memoria_tiene_pid_solicitado);
    return proceso_buscado->tabla_paginas;
}

void agregar_pagina(unsigned nro_pagina){ // me parece puedo usar la variable global PID_solicitado
    t_tabla_paginas* tabla = get_table_by_PID();
    unsigned frame_number = get_available_frame();
    marcar_frame_como(frame_number, 1); // 1 = OCUPADO
    char* nro_pagina_str = string_itoa(nro_pagina);
    dictionary_put(tabla, nro_pagina_str, (void*)frame_number); 
}

unsigned get_frame_number_by_pagina(unsigned nro_pagina){
    t_tabla_paginas* tabla = get_table_by_PID();
    char* nro_pagina_str = string_itoa(nro_pagina);
    unsigned frame_number = (unsigned) dictionary_get(tabla, nro_pagina_str);
    return frame_number;
}

unsigned calcular_cantidad_de_paginas_por_bytes(unsigned bytes){
    return (bytes + config.tam_pagina - 1) / config.tam_pagina; // división entera con redondeo hacia arriba
}