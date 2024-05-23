#include "../include/planificador_corto.h"

void inicializar_cola_ready(){
    colaReady = queue_create();
}

void *iniciar_planificacion_corto(){
    while(1){
        if(planificacion_activada){        
            sem_wait(&sem_proceso_en_ready);  

            pcb* pcb = queue_pop(colaReady);
            pcb->estado = EXECUTE;

            dispatch_proceso_planificador(pcb);
        }
    }
}

void dispatch_proceso_planificador(pcb* newPcb){
    enviar_pcb(newPcb, socket_cpu, DISPATCH_PROCESO); 
	log_info(logger, "Solicitud DISPATCH_PROCESO enviada a CPU");
}