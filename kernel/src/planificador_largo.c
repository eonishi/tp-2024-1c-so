#include "../include/planificador_largo.h"

void inicializar_cola_new(){
    colaNew = queue_create();
}

void inicializar_cola_ready(){
    colaReady = queue_create();
}

void insertar_en_cola_new(pcb* pcb){
    queue_push(colaNew, pcb);
}

void *iniciar_planificacion_largo(){
    while(1){
        log_info(logger, "While planificador largo");
        if(planificador_largo_activado){
            log_info(logger, "Esperando nuevo proceso...");
            sem_wait(&sem_nuevo_proceso);  
              
            log_info(logger, "Ingresó un nuevo proceso");
            pcb* newPcb = queue_pop(colaNew);
            log_info(logger, "Push en cola ready");
            queue_push(colaReady, newPcb);

            log_info(logger, "Dispatching to cpu");
            dispatch_proceso_planificador(newPcb);
        }
    }
}

void dispatch_proceso_planificador(pcb* newPcb){
    enviar_pcb(newPcb, socket_cpu, DISPATCH_PROCESO); 
	log_info(logger, "Solicitud DISPATCH_PROCESO enviada a CPU");
	pcb* pcb_respuesta = esperar_pcb(socket_cpu, DISPATCH_PROCESO);
	loggear_pcb(pcb_respuesta);
	log_info(logger, "Respuesta DISPATCH_PROCESO recibida");
}