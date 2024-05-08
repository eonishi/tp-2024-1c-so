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
