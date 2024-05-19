#include "../include/execute.h"

void execute(char **instr_tokenizada)
{
    operacion OP = get_operacion(instr_tokenizada);
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
    case IO_GEN_SLEEP:
        exec_io_gen_sleep(instr_tokenizada);
        siguiente_pc(pcb_actual);
        break;
    default:
        log_error(logger, "MAN esta operación no existe (¬_¬\")");
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

void exec_io_gen_sleep(char** instr_tokenizada){
    // IO_GEN_SLEEP interfaz unidades_trabajo
    //TODO
}