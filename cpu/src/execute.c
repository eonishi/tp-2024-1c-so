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