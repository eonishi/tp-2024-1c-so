#include "../include/execute.h"

void execute(char **instr_tokenizada)
{
    operacion OP = get_operacion(instr_tokenizada);
    switch (OP)
    {
    case SET:
        exec_set(instr_tokenizada);
        break;
    case SUM:
        exec_sum(instr_tokenizada);
        break;
    case SUB:
        exec_sub(instr_tokenizada);
        break;
    case JNZ:
        /* code */
        break;
    case IO_GEN_SLEEP:
        /* code */
        break;
    default:
        log_error(logger, "MAN esta operación no existe (¬_¬\")");
        break;
    }
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
        *((uint8_t*) ptr_reg_D) += *((unsigned*)ptr_reg_O);
    }
    else{
        *((uint32_t*) ptr_reg_D) += *((unsigned*) ptr_reg_O);
    }
}

void exec_sub(char** instr_tokenizada){
    // SUB reg_D reg_O
    void* ptr_reg_D = get_registro(instr_tokenizada[1]);
    void* ptr_reg_O = get_registro(instr_tokenizada[2]);
    if (es8int(instr_tokenizada[1])){
        *((uint8_t*) ptr_reg_D) -= *((unsigned*)ptr_reg_O);
    }
    else{
        *((uint32_t*) ptr_reg_D) -= *((unsigned*) ptr_reg_O);
    }
}