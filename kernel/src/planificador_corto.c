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
                        
            gestionar_respuesta_cpu();
        }
    }
}


void dispatch_proceso_planificador(pcb* newPcb){
    enviar_pcb(newPcb, socket_cpu, DISPATCH_PROCESO); 
	log_info(logger, "Solicitud DISPATCH_PROCESO enviada a CPU");    
}

void gestionar_respuesta_cpu(){
	t_list* lista;
	bool on = 1;
	while (on) {
		int cod_op = recibir_operacion(socket_cpu);
        log_info(logger, "Codigo recibido desde el cpu: [%d]", cod_op);

		switch (cod_op) {
		case PROCESO_TERMINADO:
            log_info(logger, "Recibi PROCESO_TERMINADO. CODIGO: %d", cod_op);
			pcb* pcb = recibir_pcb(socket_cpu);
			loggear_pcb(pcb);
			// Push cola exit
			break;		
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			on = 0;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
}