#include "include/cpu.h"

int main(){
    t_log* logger = iniciar_logger("cpu.log", "CPU");

    log_info(logger, "Logger CPU Iniciado");

    return 0;
}

/*
El módulo CPU en nuestro contexto de TP lo que va a hacer es simular los pasos del ciclo de instrucción de una CPU real, 
de una forma mucho más simplificada.
*/