#include "../include/execute.h"

void execute(char **instr_tokenizada)
{   
    operacion OP = get_operacion(instr_tokenizada);

    log_info(logger, "Instrucción a ejecutar: [%s]", instr_tokenizada[0]);

    switch (OP)
    {
    case SET:
        log_info(logger_oblig, "PID: <%d> - Ejecutando: <%s> - <%s %s>", pcb_actual->pid, instr_tokenizada[0],
        instr_tokenizada[1], instr_tokenizada[2]);

        exec_set(instr_tokenizada);
        siguiente_pc(pcb_actual);
        break;
    case SUM:
        log_info(logger_oblig, "PID: <%d> - Ejecutando: <%s> - <%s %s>", pcb_actual->pid, instr_tokenizada[0],
        instr_tokenizada[1], instr_tokenizada[2]);

        exec_sum(instr_tokenizada);
        siguiente_pc(pcb_actual);
        break;
    case SUB:
        log_info(logger_oblig, "PID: <%d> - Ejecutando: <%s> - <%s %s>", pcb_actual->pid, instr_tokenizada[0],
        instr_tokenizada[1], instr_tokenizada[2]);

        exec_sub(instr_tokenizada);
        siguiente_pc(pcb_actual);
        break;
    case JNZ:
        log_info(logger_oblig, "PID: <%d> - Ejecutando: <%s> - <%s %s>", pcb_actual->pid, instr_tokenizada[0],
        instr_tokenizada[1], instr_tokenizada[2]);

        exec_jnz(instr_tokenizada);
        break;
    case RESIZE:
        log_info(logger_oblig, "PID: <%d> - Ejecutando: <%s> - <%s>", pcb_actual->pid, instr_tokenizada[0],
        instr_tokenizada[1]);
        exec_resize(instr_tokenizada);
        siguiente_pc(pcb_actual);
        break;
    case MOV_OUT:
        log_info(logger_oblig, "PID: <%d> - Ejecutando: <%s> - <%s %s>", pcb_actual->pid, instr_tokenizada[0],
        instr_tokenizada[1], instr_tokenizada[2]);

        exec_mov_out(instr_tokenizada);
        siguiente_pc(pcb_actual);
        break;
    case MOV_IN:
        log_info(logger_oblig, "PID: <%d> - Ejecutando: <%s> - <%s %s>", pcb_actual->pid, instr_tokenizada[0],
        instr_tokenizada[1], instr_tokenizada[2]);

        exec_mov_in(instr_tokenizada);
        siguiente_pc(pcb_actual);
        break;
    case COPY_STRING:
        log_info(logger_oblig, "PID: <%d> - Ejecutando: <%s> - <%s>", pcb_actual->pid, instr_tokenizada[0],
        instr_tokenizada[1]);

        exec_cp_string(instr_tokenizada);
        siguiente_pc(pcb_actual);
        break;
    case IO_FS_TRUNCATE:
        log_info(logger_oblig, "PID: <%d> - Ejecutando: <%s> - <%s %s %s>", pcb_actual->pid, instr_tokenizada[0],
        instr_tokenizada[1],instr_tokenizada[2],instr_tokenizada[3]);

        uint32_t dato_registro = reg_to_uint32(get_registro(instr_tokenizada[3]), instr_tokenizada[3]);

        reemplazar_registro_por_dato(instr_tokenizada, 3, dato_registro);
        exec_operacion_io(instr_tokenizada);                
        break;
    case IO_FS_READ:
    case IO_FS_WRITE:
        log_info(logger_oblig, "PID: <%d> - Ejecutando: <%s> - <%s %s %s %s %s>", pcb_actual->pid, instr_tokenizada[0],
        instr_tokenizada[1],instr_tokenizada[2],instr_tokenizada[3],instr_tokenizada[4], instr_tokenizada[5]);

        tengo_pcb = 0;
        siguiente_pc(pcb_actual);
        exec_io_fs_read_write(instr_tokenizada);         
        break;
    case IO_GEN_SLEEP:
        log_info(logger_oblig, "PID: <%d> - Ejecutando: <%s> - <%s %s>", pcb_actual->pid, instr_tokenizada[0],
        instr_tokenizada[1], instr_tokenizada[2]);

        exec_operacion_io(instr_tokenizada);                
        break;
    case IO_FS_CREATE:
        log_info(logger_oblig, "PID: <%d> - Ejecutando: <%s> - <%s %s>", pcb_actual->pid, instr_tokenizada[0],
        instr_tokenizada[1], instr_tokenizada[2]);

        exec_operacion_io(instr_tokenizada);                
        break;
    case IO_FS_DELETE:
        log_info(logger_oblig, "PID: <%d> - Ejecutando: <%s> - <%s %s>", pcb_actual->pid, instr_tokenizada[0],
        instr_tokenizada[1], instr_tokenizada[2]);

        exec_operacion_io(instr_tokenizada);                
        break;
    case IO_STDIN_READ:
        log_info(logger_oblig, "PID: <%d> - Ejecutando: <%s> - <%s %s %s>", pcb_actual->pid, instr_tokenizada[0],
        instr_tokenizada[1],instr_tokenizada[2],instr_tokenizada[3]);

        tengo_pcb = 0;
        siguiente_pc(pcb_actual);
        
        exec_io_stdin_read(instr_tokenizada);
        break;     
    case IO_STDOUT_WRITE:
        log_info(logger_oblig, "PID: <%d> - Ejecutando: <%s> - <%s %s %s>", pcb_actual->pid, instr_tokenizada[0],
        instr_tokenizada[1],instr_tokenizada[2],instr_tokenizada[3]);

        tengo_pcb = 0;
        siguiente_pc(pcb_actual);
        
        exec_io_stdout_write(instr_tokenizada);
        break;
        
    case SIGNAL:
        log_info(logger_oblig, "PID: <%d> - Ejecutando: <%s> - <%s>", pcb_actual->pid, instr_tokenizada[0],
        instr_tokenizada[1]);

        desalojar_pcb(instr_tokenizada, PROCESO_LIBERA_RECURSO);
        break;

    case WAIT:
        log_info(logger_oblig, "PID: <%d> - Ejecutando: <%s> - <%s>", pcb_actual->pid, instr_tokenizada[0],
        instr_tokenizada[1]);

        desalojar_pcb(instr_tokenizada, PROCESO_SOLICITA_RECURSO);
        break;

    case EXIT_OP:
        tengo_pcb = 0;
        enviar_pcb(pcb_actual, socket_kernel, PROCESO_TERMINADO);
        break;
    default:
        log_error(logger, "MAN esta operación no existe (¬_¬\")");
        tengo_pcb = 0;
        enviar_pcb(pcb_actual, socket_kernel, ERROR_DE_PROCESAMIENTO);
        log_error(logger, "Devolví el PCB al kernel.");
        break;
    }

    free(instr_tokenizada);
}

void reemplazar_registro_por_dato(char** instr_tokenizada, int index, int valor){
    // Calcula el tamaño necesario para la cadena
    size_t buffer_size = snprintf(NULL, 0, "%u", valor) + 1;

    // Asigna suficiente espacio para la cadena resultante
    instr_tokenizada[index] = realloc(instr_tokenizada[3], buffer_size);
    if (instr_tokenizada[index] == NULL) {
        log_error(logger, "Error al intentar reemplazar registro por dato");
        enviar_pcb(pcb_actual, socket_kernel, ERROR_DE_PROCESAMIENTO);
    }

    // Convierte dato_registro a cadena y guarda en instr_tokenizada[3]
    snprintf(instr_tokenizada[index], buffer_size, "%u", valor);
}

void exec_set(char **instr_tokenizada){
    // SET registro valor
    void* ptr_registro = get_registro(instr_tokenizada[1]);
    int valor = get_valor(instr_tokenizada[2]);
    if (es8int(instr_tokenizada[1])){
        *((uint8_t*) ptr_registro) = valor;
    }
    else{
        *((uint32_t*) ptr_registro) = valor;
    }
}

void exec_sum(char** instr_tokenizada){
    // SUM reg_D reg_O
    void* ptr_reg_D = get_registro(instr_tokenizada[1]);
    void* ptr_reg_O = get_registro(instr_tokenizada[2]); 
    if (es8int(instr_tokenizada[1])){
        if(es8int(instr_tokenizada[2])){
            *((uint8_t*) ptr_reg_D) += *((uint8_t*)ptr_reg_O);
        }
        else{
            *((uint8_t*) ptr_reg_D) += *((uint32_t*)ptr_reg_O);
        }
    }
    else if(es8int(instr_tokenizada[2])){
        *((uint32_t*) ptr_reg_D) += *((uint8_t*) ptr_reg_O);
    }
    else {
        *((uint32_t*) ptr_reg_D) += *((uint32_t*) ptr_reg_O);
    }
}

void exec_sub(char** instr_tokenizada){
    // SUB reg_D reg_O
    void* ptr_reg_D = get_registro(instr_tokenizada[1]);
    void* ptr_reg_O = get_registro(instr_tokenizada[2]); 
    if (es8int(instr_tokenizada[1])){
        if(es8int(instr_tokenizada[2])){
            *((uint8_t*) ptr_reg_D) -= *((uint8_t*)ptr_reg_O);
        }
        else{
            *((uint8_t*) ptr_reg_D) -= *((uint32_t*)ptr_reg_O);
        }
    }
    else if(es8int(instr_tokenizada[2])){
        *((uint32_t*) ptr_reg_D) -= *((uint8_t*) ptr_reg_O);
    }
    else {
        *((uint32_t*) ptr_reg_D) -= *((uint32_t*) ptr_reg_O);
    }
}

void exec_jnz(char** instr_tokenizada){
    // JNZ registro valor
    void* ptr_registro = get_registro(instr_tokenizada[1]);
    int valor = get_valor(instr_tokenizada[2]);
    if(es8int(instr_tokenizada[1])){ // DIOS demasiados IFFFF
        if (*((uint8_t*) ptr_registro) != 0){
            pcb_actual->pc = valor;
        }
    }
    else{
        if (*((uint32_t*) ptr_registro) != 0){
            pcb_actual->pc = valor;
        }
    }
}

void desalojar_pcb(char** instr_tokenizada, op_code codigo){
    tengo_pcb = 0;
    siguiente_pc(pcb_actual);
    pcb_actual->solicitud = crear_solicitud_io(instr_tokenizada, NULL);
    enviar_pcb(pcb_actual, socket_kernel, codigo);
}

void exec_operacion_io(char** instr_tokenizada){
    desalojar_pcb(instr_tokenizada, PROCESO_BLOQUEADO_IO);
}

void controlar_peticion_a_memoria(){
    op_code status = recibir_operacion(socket_memoria);
    switch (status)
    {
        case SUCCESS:
            log_info(logger, "La operacion en memoria fue exitosa");
            break;
    
        case OUT_OF_MEMORY:
            log_error(logger, "No hay suficiente memoria para realizar la operacion");
            tengo_pcb = 0;
            enviar_pcb(pcb_actual, socket_kernel, OUT_OF_MEMORY);
            break;
            
        default:
            break;
            log_error(logger, "Hubo un problema con la operacion en memoria");
            tengo_pcb = 0;
            enviar_pcb(pcb_actual, socket_kernel, ERROR_DE_PROCESAMIENTO);
    }
}

void exec_resize(char** instr_tokenizada){
    // RESIZE cantidad_paginas

    int tamanio_en_bytes = get_valor(instr_tokenizada[1]);
    enviar_cantidad(tamanio_en_bytes, REDIMENSIONAR_MEMORIA_PROCESO, socket_memoria);

    controlar_peticion_a_memoria();
}

/**
 * MOV_OUT (Registro Dirección, Registro Datos): 
 * Lee el valor del Registro Datos y lo escribe en la dirección física de memoria 
 * obtenida a partir de la Dirección Lógica almacenada en el Registro Dirección.
 * **/
void enviar_peticiones_de_escribir(void* peticion){
    t_peticion_memoria* peticion_a_enviar = (t_peticion_memoria*) peticion;
    peticion_escritura_enviar(peticion_a_enviar, socket_memoria);
    controlar_peticion_a_memoria();
}

void exec_mov_out(char** instr_tokenizada){
    log_info(logger, "Inicia exec_mov_out");

    char* registro_direccion = instr_tokenizada[1];
    char* registro_datos = instr_tokenizada[2];
    log_info(logger, "Registro dirección: [%s], Registro datos: [%s]", registro_direccion, registro_datos);

    uint32_t direccion_logica = reg_to_uint32(get_registro(registro_direccion), registro_direccion);
    void* dato_a_escribir = get_registro(registro_datos);
    log_info(logger, "Dirección logica: [%d], Dato a escribir: [%d]", direccion_logica, dato_a_escribir);

    t_list* direcciones_fisicas = mmu(direccion_logica, tam_registro(registro_datos), dato_a_escribir);
    t_peticion_memoria* primer_peticion = (t_peticion_memoria*) list_get(direcciones_fisicas, 0);

    if(es8int(registro_datos)){
        log_info(logger_oblig, "PID: <%d> - Acción: <ESCRIBIR> - Dirección Física: <%d> - Valor: <%u>",
        pcb_actual->pid, primer_peticion->direccion_fisica, *(uint8_t*) dato_a_escribir);
        
    }
    else{
        log_info(logger_oblig, "PID: <%d> - Acción: <ESCRIBIR> - Dirección Física: <%d> - Valor: <%u>",
        pcb_actual->pid, primer_peticion->direccion_fisica, *(uint32_t*) dato_a_escribir);
    }

    list_iterate(direcciones_fisicas, enviar_peticiones_de_escribir);
}

// MOV_IN (Registro Datos, Registro Dirección)
/* 
 Lee el valor de memoria correspondiente a la Dirección Lógica que se encuentra en el Registro Dirección 
 y lo almacena en el Registro Datos.
*/
void leer_dato_memoria(t_peticion_memoria* peticion_a_enviar, void** ptr_donde_se_guarda_el_dato){
    peticion_lectura_enviar(peticion_a_enviar, ptr_donde_se_guarda_el_dato, socket_memoria);
    controlar_peticion_a_memoria();
}

void exec_mov_in(char** instr_tokenizada){
    log_info(logger, "Inicia exec_mov_in");

    char* registro_datos = instr_tokenizada[1];
    char* registro_direccion = instr_tokenizada[2];
    
    log_info(logger, "Registro datos: [%s], Registro dirección: [%s]", registro_datos, registro_direccion);

    uint32_t direccion_logica = reg_to_uint32(get_registro(registro_direccion), registro_direccion);
    void* registro_que_guarda_dato = get_registro(registro_datos);

    log_info(logger, "Dirección logica: [%d], Valor actual en registro de datos: [%d]", direccion_logica, registro_que_guarda_dato);
    
    t_list *direcciones_fisicas = mmu(direccion_logica, tam_registro(registro_datos), NULL);

    for(int i=0; i<list_size(direcciones_fisicas); i++){
        t_peticion_memoria* peticion_a_enviar = list_get(direcciones_fisicas, i);
        leer_dato_memoria(peticion_a_enviar, &registro_que_guarda_dato);
    }

    t_peticion_memoria* primer_peticion = (t_peticion_memoria*) list_get(direcciones_fisicas, 0);

    if(es8int(registro_datos)){
        log_info(logger_oblig, "PID: <%d> - Acción: <ESCRIBIR> - Dirección Física: <%d> - Valor: <%u>",
        pcb_actual->pid, primer_peticion->direccion_fisica, *(uint8_t*) registro_que_guarda_dato);
        
    }
    else{
        log_info(logger_oblig, "PID: <%d> - Acción: <ESCRIBIR> - Dirección Física: <%d> - Valor: <%u>",
        pcb_actual->pid, primer_peticion->direccion_fisica, *(uint32_t*) registro_que_guarda_dato);
    }


    log_info(logger, "Dato guardado en ,mel registro: [%d]", registro_que_guarda_dato);
}

void exec_cp_string(char** instr_tokenizada){
    int tam_string = get_valor(instr_tokenizada[1]);
    uint32_t dl_src = pcb_actual->registros->si;
    uint32_t dl_dst = pcb_actual->registros->di;
    void* string_copiado = malloc(tam_string);
    void* ptr_string_copiado = string_copiado;

    // Leer el string de la dirección lógica dl_src
    t_list *peticiones_lectura = mmu(dl_src, tam_string, NULL);
    for(int i=0; i < list_size(peticiones_lectura); i++){
        t_peticion_memoria* peticion_a_enviar = list_get(peticiones_lectura, i);
        leer_dato_memoria(peticion_a_enviar, &ptr_string_copiado);
    }
      // Escribir el string en la dirección lógica dl_dst
    t_list *peticiones_escritura = mmu(dl_dst, tam_string, string_copiado);
    list_iterate(peticiones_escritura, enviar_peticiones_de_escribir);

    log_info(logger, "String copiado: [%s] desde [%d] a [%d]", string_copiado, dl_src, dl_dst);
    free(string_copiado);
    // Despues hay que liberar todas las peticiones tambien para mov_in y mov_out
}

/*
IO_STDIN_READ (Interfaz, Registro Dirección, Registro Tamaño): Esta instrucción solicita al Kernel que mediante 
la interfaz ingresada se lea desde el STDIN (Teclado) un valor cuyo tamaño está delimitado por el valor del 
Registro Tamaño y el mismo se guarde a partir de la Dirección Lógica almacenada en el Registro Dirección.
*/
void exec_io_stdin_read(char** instr_tokenizada){
   // IO_STDIN_READ, Interfaz, Registro Direccion, Registro Tamaño
    char* registro_direccion = instr_tokenizada[2];
    char* registro_tamanio = instr_tokenizada[3];
    log_info(logger, "Registro dirección: [%s], Registro Tamanio: [%s]", registro_direccion, registro_tamanio);


    uint32_t direccion_logica = reg_to_uint32(get_registro(registro_direccion), registro_direccion);
    uint32_t tamanio = reg_to_uint32(get_registro(registro_tamanio), registro_tamanio);

    log_info(logger, "Dirección logica: [%d], Tamaño: [%d]", direccion_logica, tamanio);

    t_list *peticiones_lectura = mmu(direccion_logica, tamanio, NULL);

    solicitud_bloqueo_por_io* solicitud = crear_solicitud_io(instr_tokenizada, peticiones_lectura);
    pcb_actual->solicitud = solicitud;

    enviar_pcb(pcb_actual, socket_kernel, PROCESO_BLOQUEADO_IO);
}

void exec_io_stdout_write(char** instr_tokenizada){
    // IO_STDOUT_WRITE, Interfaz, Registro Direccion, Registro Tamaño
    char* registro_direccion = instr_tokenizada[2];
    char* registro_tamanio = instr_tokenizada[3];
    log_info(logger, "Registro dirección: [%s], Registro Tamanio: [%s]", registro_direccion, registro_tamanio);

    uint32_t direccion_logica = reg_to_uint32(get_registro(registro_direccion), registro_direccion);
    uint32_t tamanio = reg_to_uint32(get_registro(registro_tamanio), registro_tamanio);
    log_info(logger, "Dirección logica: [%d], Tamaño: [%d]", direccion_logica, tamanio);

    t_list *peticiones_lectura = mmu(direccion_logica, tamanio, NULL);

    solicitud_bloqueo_por_io* solicitud = crear_solicitud_io(instr_tokenizada, peticiones_lectura);
    pcb_actual->solicitud = solicitud;

    enviar_pcb(pcb_actual, socket_kernel, PROCESO_BLOQUEADO_IO);
}

/* IO_FS_WRITE (Interfaz, Nombre Archivo, Registro Dirección, Registro Tamaño, Registro Puntero Archivo): 
        Esta instrucción solicita al Kernel que mediante la interfaz seleccionada, 
        se lea desde Memoria la cantidad de bytes indicadas por el Registro Tamaño a partir de la 
        dirección lógica que se encuentra en el Registro Dirección y se escriban en el archivo 
        a partir del valor del Registro Puntero Archivo.
 */
void exec_io_fs_read_write(char** instr_tokenizada){    
    char* registro_direccion = instr_tokenizada[3];
    char* registro_tamanio = instr_tokenizada[4];
    char* registro_puntero_archivo = instr_tokenizada[5];
    
    log_info(logger, "Registro dirección: [%s], Registro Tamanio: [%s], Reg Puntero archivo: [%s]", registro_direccion, registro_tamanio, registro_puntero_archivo);

    uint32_t direccion_logica = reg_to_uint32(get_registro(registro_direccion), registro_direccion);
    uint32_t tamanio = reg_to_uint32(get_registro(registro_tamanio), registro_tamanio);
    int32_t puntero_archivo = reg_to_uint32(get_registro(registro_puntero_archivo), registro_puntero_archivo);

    reemplazar_registro_por_dato(instr_tokenizada, 5, puntero_archivo);

    log_info(logger, "Dirección logica: [%d], Tamaño: [%d], Puntero Archivo: [%d]", direccion_logica, tamanio, puntero_archivo);

    t_list *peticiones_lectura = mmu(direccion_logica, tamanio, NULL);    

    solicitud_bloqueo_por_io* solicitud = crear_solicitud_io(instr_tokenizada, peticiones_lectura);
    pcb_actual->solicitud = solicitud;

    enviar_pcb(pcb_actual, socket_kernel, PROCESO_BLOQUEADO_IO);
}