#include "../include/checkInterrupt.h"

void checkInterrupt(){
    if(INTERRUPTION_FLAG){
        log_info(logger, "RECIBI UNA INTERRUPCION.");
        INTERRUPTION_FLAG = 0;
        tengo_pcb = 0; // Envio el pcb a kernel, entonces no tengo más el pcb para ejecutar, corta el ciclo de cpu.

        enviar_pcb(pcb_actual, socket_kernel, MOTIVO_INTERRUPCION);
    } 
}