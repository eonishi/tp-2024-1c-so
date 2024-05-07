#include "../include/execute.h" 

void execute(char** instruccion_tokenizada){
    operacion OP = get_operacion(instruccion_tokenizada);
    switch (OP)
    {
    case SET:
        /* code */
        break;
    case SUM:
        /* code */
        break;
    case SUB:
        /* code */
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

operacion get_operacion(char** inst_tokenizada){
    if(string_equals_ignore_case(inst_tokenizada[0], "SET")){
        return SET;
    } else if(string_equals_ignore_case(inst_tokenizada[0], "SUM")){
        return SUM;
    } else if(string_equals_ignore_case(inst_tokenizada[0], "SUB")){
        return SUB;
    } else if(string_equals_ignore_case(inst_tokenizada[0], "JNZ")){
        return JNZ;
    } else if(string_equals_ignore_case(inst_tokenizada[0], "IO_GEN_SLEEP")){
        return IO_GEN_SLEEP;
    }
}