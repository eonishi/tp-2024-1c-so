#include "../include/fetch.h"

char* fetch(){
    pedir_intruccion_a_memoria(pcb_actual->pc);
    return recibir_instruccion_de_memoria(socket_memoria);
}

char *recibir_instruccion_de_memoria(int socket_memoria){
    int codigo_recibido = recibir_operacion(socket_memoria);
    if(codigo_recibido != FETCH_INSTRUCCION){
        log_error(logger, "El código de respuesta no es el esperado. Esperado: [%d]. Recibido: [%d]", FETCH_INSTRUCCION, codigo_recibido);
        return "FAIL";
    }

    int size;
    char *buffer = recibir_buffer(&size, socket_memoria);
    log_info(logger, "Mensaje recibido: [%s]", buffer);
    return buffer;
}

void pedir_intruccion_a_memoria(unsigned PC){ // Probablemente haya que mandar tambien el PID
    //TODO solicitar instruccion a memoria enviando PC y PID (no un string de PC)
    enviar_mensaje(FETCH_INSTRUCCION, string_itoa(PC), socket_memoria);
    log_info(logger, "Solicitud FETCH_INSTRUCCION enviada a memoria");
}