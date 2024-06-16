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
        exec_io_stdin_read(instr_tokenizada);
        siguiente_pc(pcb_actual);
        break;     
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

void exec_resize(char** instr_tokenizada){
    // RESIZE cantidad_paginas

    int tamanio_en_bytes = get_valor(instr_tokenizada[1]);
    enviar_cantidad(tamanio_en_bytes, REDIMENSIONAR_MEMORIA_PROCESO, socket_memoria);

    // Esperar confirmación de la memoria
    op_code status = recibir_operacion(socket_memoria);
    if(status == SUCCESS){
        log_info(logger, "Se redimensionó la memoria correctamente");
    }
    else{
        log_error(logger, "Hubo un problema al redimensionar la memoria");
        tengo_pcb = 0;
        enviar_pcb(pcb_actual, socket_kernel, ERROR_DE_PROCESAMIENTO); // o OUT_OF_MEMORY?
    }
}

/**
 * MOV_OUT (Registro Dirección, Registro Datos): 
 * Lee el valor del Registro Datos y lo escribe en la dirección física de memoria 
 * obtenida a partir de la Dirección Lógica almacenada en el Registro Dirección.
 * **/
void exec_mov_out(char** instr_tokenizada){
    log_info(logger, "Inicia exec_mov_out");

    char* registro_direccion = instr_tokenizada[1];
    char* registro_datos = instr_tokenizada[2];

    log_info(logger, "Registro dirección: [%s], Registro datos: [%s]", registro_direccion, registro_datos);

    uint32_t* direccion_logica = get_registro(registro_direccion);
    uint32_t* dato_a_escribir = get_registro(registro_datos);

    log_info(logger, "Dirección logica: [%d], Dato a escribir: [%d]", *direccion_logica, *dato_a_escribir);

    uint32_t direccion_fisica = obtener_direccion_fisica_de_tlb(direccion_logica);

    if (!existe_en_tlb(direccion_fisica)){
        log_info(logger, "No existe en tlb");
        direccion_fisica = calcular_direccion_fisica(direccion_logica);
    }
        
    enviar_escribir_dato_en_memoria(direccion_fisica, *dato_a_escribir, socket_memoria);

    // Esperar confirmación de la memoria
    op_code status = recibir_operacion(socket_memoria);
    if(status == SUCCESS){
        log_info(logger, "Se escribió el dato en memoria");
    }
    else{
        log_error(logger, "Hubo un problema al intentar escribir el dato en memoria");
        tengo_pcb = 0;
        enviar_pcb(pcb_actual, socket_kernel, ERROR_DE_PROCESAMIENTO); // o OUT_OF_MEMORY?
    }
}

// Se considera 0 como no existente
int existe_en_tlb(uint32_t direccion_fisica)
{
    return direccion_fisica != 0;
}

uint32_t obtener_direccion_fisica_de_tlb(uint32_t* direccion_logica){
    // TODO IMPLEMENTAR ACCESO A TLB
    uint32_t* direccion_fisica;
    *direccion_fisica = 1;

    log_info(logger, "Falta imple de obtención de dirección fisica de tlb. Return Hardcode: [%d]", *direccion_fisica);

    return *direccion_fisica;
}

uint32_t calcular_direccion_fisica(uint32_t* direccion_logica){
    log_info(logger, "entra calcular_direccion_fisica");
    // TODO IMPLEMENTAR CALCULO DE DIRECCION FISICA CON MMU
    uint32_t* direccion_fisica;
    *direccion_fisica = 1;

    log_info(logger, "Falta imple de calcular dirección fisica. Return Hardcode: [%d]", *direccion_fisica);
    

    return *direccion_fisica;
}


// MOV_IN (Registro Datos, Registro Dirección)
/* 
 Lee el valor de memoria correspondiente a la Dirección Lógica que se encuentra en el Registro Dirección 
 y lo almacena en el Registro Datos.

*/
void exec_mov_in(char** instr_tokenizada){
    log_info(logger, "Inicia exec_mov_in");

    char* registro_datos = instr_tokenizada[1];
    char* registro_direccion = instr_tokenizada[2];
    
    log_info(logger, "Registro datos: [%s], Registro dirección: [%s]", registro_datos, registro_direccion);

    uint32_t* direccion_logica = get_registro(registro_direccion);
    uint32_t* dato_en_registro = get_registro(registro_datos);

    log_info(logger, "Dirección logica: [%d], Valor actual en registro de datos: [%d]", *direccion_logica, *dato_en_registro);

    uint32_t direccion_fisica = obtener_direccion_fisica_de_tlb(direccion_logica);

    if (!existe_en_tlb(direccion_fisica)){
        log_info(logger, "No existe en tlb");
        direccion_fisica = calcular_direccion_fisica(direccion_logica);
    }
        
    enviar_solicitud_leer_dato_de_memoria(direccion_fisica, socket_memoria);

    // Esperar confirmación de la memoria
    log_info(logger, "Esperando dato leido de memoria");
    op_code status = recibir_operacion(socket_memoria);
    if(status == DATO_LEIDO_DE_MEMORIA){
        log_info(logger, "Recibi DATO_LEIDO_DE_MEMORIA");
        uint32_t dato_leido = recibir_dato_leido_de_memoria(socket_memoria);

        log_info(logger, "Se recibió el dato en memoria: [%d]", dato_leido);
    }
    else{
        log_error(logger, "Hubo un problema al intentar escribir el dato en memoria");
        tengo_pcb = 0;
        enviar_pcb(pcb_actual, socket_kernel, ERROR_DE_PROCESAMIENTO); // o OUT_OF_MEMORY?
    }
}

/*
IO_STDIN_READ (Interfaz, Registro Dirección, Registro Tamaño): Esta instrucción solicita al Kernel que mediante 
la interfaz ingresada se lea desde el STDIN (Teclado) un valor cuyo tamaño está delimitado por el valor del 
Registro Tamaño y el mismo se guarde a partir de la Dirección Lógica almacenada en el Registro Dirección.
*/
void exec_io_stdin_read(char** instr_tokenizada){
    log_info(logger, "Inicia io_stdin_read");

    char* interfaz = instr_tokenizada[1];
    char* registro_direccion = instr_tokenizada[2];
    char* registro_tamanio = instr_tokenizada[3];
    
    log_info(logger, "Interfaz: [%s], Registro dirección: [%s], Registro tamanio: [%s]", interfaz, registro_direccion, registro_tamanio);

    uint32_t* direccion_logica = get_registro(registro_direccion);
    uint32_t* tamanio_maximo = get_registro(registro_tamanio);

    log_info(logger, "Dirección logica: [%d], Tamanio maximo: [%d]", *direccion_logica, *tamanio_maximo);

    uint32_t direccion_fisica = obtener_direccion_fisica_de_tlb(direccion_logica);

    if (!existe_en_tlb(direccion_fisica)){
        log_info(logger, "No existe en tlb");
        direccion_fisica = calcular_direccion_fisica(direccion_logica);
    }
        
    sprintf(instr_tokenizada[2], "%u", direccion_fisica);
    sprintf(instr_tokenizada[3], "%u", *tamanio_maximo);

    log_info(logger, "Enviando a kernel: [%s][%s][%s][%s]",instr_tokenizada[0],instr_tokenizada[1],instr_tokenizada[2],instr_tokenizada[3]);

    exec_operacion_io(instr_tokenizada);
}