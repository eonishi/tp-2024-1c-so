#include "../include/checkInterrupt.h"

void checkInterrupt(){
    if(INTERRUPTION_FLAG){
        INTERRUPTION_FLAG = 0;
        tengo_pcb = 0; // Envio el pcb a kernel, entonces no tengo más el pcb para ejecutar
        enviar_pcb(pcb_actual, socket_kernel, INTERRUPCION);
    }
}