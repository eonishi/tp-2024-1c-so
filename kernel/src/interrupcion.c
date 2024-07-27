#include "../include/interrupcion.h"

void send_interrupt(unsigned PID, op_code motivo_interrupt, int socket_interrupt){
    enviar_cantidad(PID, motivo_interrupt, socket_interrupt);
	log_info(logger, "Solicitud INTERRUPCION del PID [%d] enviada a CPU", PID);
}

void interrumpir_proceso_ejecutando(op_code motivo_interrupt){
    if(queue_is_empty(cola_execute)){
        log_info(logger, "No hay procesos en ejecución");
        return;
    }
    
    pcb* pcb = queue_peek(cola_execute);
    send_interrupt(pcb->pid,motivo_interrupt, socket_cpu_interrupt);
}