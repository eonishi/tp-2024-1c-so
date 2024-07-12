#include "../include/interrupcion.h"

void send_interrupt(unsigned PID, int socket_interrupt){
    enviar_cantidad(PID, INTERRUPCION, socket_interrupt);
	log_info(logger, "Solicitud INTERRUPCION del PID [%d] enviada a CPU", PID);
}

void interrumpir_proceso_ejecutando(){
    pcb* pcb = queue_peek(cola_execute);
    send_interrupt(pcb->pid, socket_cpu_interrupt);
}