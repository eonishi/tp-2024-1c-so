#include "../include/execute.h"

void execute(char **instr_tokenizada)
{   
    operacion OP = get_operacion(instr_tokenizada);

    log_info(logger, "Instrucción a ejecutar: [%s]", instr_tokenizada[0]);

    switch (OP)
    {
    case SET:
        exec_set(instr_tokenizada);
        siguiente_pc(pcb_actual);
        break;
    case SUM:
        exec_sum(instr_tokenizada);
        siguiente_pc(pcb_actual);
        break;
    case SUB:
        exec_sub(instr_tokenizada);
        siguiente_pc(pcb_actual);
        break;
    case JNZ:
        exec_jnz(instr_tokenizada);
        break;
    case RESIZE:
        exec_resize(instr_tokenizada);
        siguiente_pc(pcb_actual);
        break;
    case MOV_OUT:
        exec_mov_out(instr_tokenizada);
        siguiente_pc(pcb_actual);
        break;
    case MOV_IN:
        exec_mov_in(instr_tokenizada);
        siguiente_pc(pcb_actual);
        break;
    case COPY_STRING:
        exec_cp_string(instr_tokenizada);
        siguiente_pc(pcb_actual);
        break;
    case IO_GEN_SLEEP:
    case IO_FS_CREATE:
    case IO_FS_DELETE:
    case IO_FS_TRUNCATE:
    case IO_FS_WRITE:
    case IO_FS_READ:
        tengo_pcb = 0;
        siguiente_pc(pcb_actual);
        exec_operacion_io(instr_tokenizada);                
        break;
    case IO_STDIN_READ:
    case IO_STDOUT_WRITE:
        tengo_pcb = 0;
        siguiente_pc(pcb_actual);
        
        exec_operacion_io(instr_tokenizada);
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

void exec_operacion_io(char** instr_tokenizada){
    solicitud_bloqueo_por_io solicitud;
    solicitud.instruc_io_tokenizadas = instr_tokenizada;
    solicitud.pcb = pcb_actual;

    enviar_bloqueo_por_io(solicitud, socket_kernel);
}

void controlar_peticion_a_memoria(){
    op_code status = recibir_operacion(socket_memoria);
    if(status == SUCCESS){
        log_info(logger, "La operacion en memoria fue exitosa");
    }
    else{
        log_error(logger, "Hubo un problema con la operacion en memoria");
        tengo_pcb = 0;
        enviar_pcb(pcb_actual, socket_kernel, ERROR_DE_PROCESAMIENTO); // o OUT_OF_MEMORY?
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
    peticion_escritura_enviar(peticion, socket_memoria);
    controlar_peticion_a_memoria();
}

void exec_mov_out(char** instr_tokenizada){
    log_info(logger, "Inicia exec_mov_out");

    char* registro_direccion = instr_tokenizada[1];
    char* registro_datos = instr_tokenizada[2];
    log_info(logger, "Registro dirección: [%s], Registro datos: [%s]", registro_direccion, registro_datos);

    uint32_t* direccion_logica = get_registro(registro_direccion);
    void* dato_a_escribir = get_registro(registro_datos);
    log_info(logger, "Dirección logica: [%d], Dato a escribir: [%d]", *direccion_logica, dato_a_escribir);

    t_list* direcciones_fisicas = mmu(*direccion_logica, tam_registro(registro_datos), dato_a_escribir);
    list_iterate(direcciones_fisicas, enviar_peticiones_de_escribir);
}

// MOV_IN (Registro Datos, Registro Dirección)
/* 
 Lee el valor de memoria correspondiente a la Dirección Lógica que se encuentra en el Registro Dirección 
 y lo almacena en el Registro Datos.
*/
void leer_dato_memoria(t_peticion_memoria* peticion_a_enviar, void** ptr_donde_se_guarda_el_dato){
        log_info(logger, "Enviando solicitud de leer a memoria. Dirección: [%d], Tam_Dato: [%d]", peticion_a_enviar->direccion_fisica, peticion_a_enviar->tam_dato);
        peticion_enviar(peticion_a_enviar, LEER_DATO_DE_MEMORIA, socket_memoria);

        int size;
        void *parte_del_dato = recibir_buffer(&size, socket_memoria);
        memcpy(*ptr_donde_se_guarda_el_dato, parte_del_dato, size);
        *ptr_donde_se_guarda_el_dato += size;
        free(parte_del_dato);

        controlar_peticion_a_memoria();
}

void exec_mov_in(char** instr_tokenizada){
    log_info(logger, "Inicia exec_mov_in");

    char* registro_datos = instr_tokenizada[1];
    char* registro_direccion = instr_tokenizada[2];
    
    log_info(logger, "Registro datos: [%s], Registro dirección: [%s]", registro_datos, registro_direccion);

    uint32_t* direccion_logica = get_registro(registro_direccion);
    void* registro_que_guarda_dato = get_registro(registro_datos);

    log_info(logger, "Dirección logica: [%d], Valor actual en registro de datos: [%d]", *direccion_logica, registro_que_guarda_dato);

    t_list *direcciones_fisicas = mmu(*direccion_logica, tam_registro(registro_datos), NULL);
    for(int i=0; i<list_size(direcciones_fisicas); i++){
        t_peticion_memoria* peticion_a_enviar = list_get(direcciones_fisicas, i);
        leer_dato_memoria(peticion_a_enviar, &registro_que_guarda_dato);
    }

    log_info(logger, "Dato guardado en el registro: [%d]", registro_que_guarda_dato);
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