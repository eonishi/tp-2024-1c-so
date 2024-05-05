#include "../include/decode.h"

char **decode(char *instruccion)
{
    // Ahora solo se tokeniza la instruccion, dsp decode tiene que ir a MMU si lo necesitase?
    char **instruccion_tokenizada = tokenizar_instruccion(instruccion);
    return instruccion_tokenizada;
}

char **tokenizar_instruccion(char *instruccion)
{
    char **instruccion_tokenizada = string_array_new();
    instruccion_tokenizada = string_split(instruccion, " ");
    return instruccion_tokenizada;
    //[Operacion, Operando1, ..., OperandoN]
}