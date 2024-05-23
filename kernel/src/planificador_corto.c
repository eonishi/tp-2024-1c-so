#include "../include/planificador_corto.h"

void *iniciar_planificacion_corto(){
    while(1){
        if(planificacion_activada){   
			log_info(logger, "Corto: Esperando otro proceso en ready");					
            sem_wait(&sem_proceso_en_ready);  			
			log_info(logger, "Corto: Llegó proceso en ready");		
			log_info(logger, "Corto: Esperando que el cpu este libre...");	
			sem_wait(&sem_cpu_libre);
			log_info(logger, "Corto: Cpu libre! pasando proximo proceso a execute..");	

            pcb* pcb = pop_cola_ready();
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

	int cod_op = recibir_operacion(socket_cpu);
	log_info(logger, "Codigo recibido desde el cpu: [%d]", cod_op);

	switch (cod_op) {
	case PROCESO_TERMINADO:
		log_info(logger, "Recibi PROCESO_TERMINADO. CODIGO: %d", cod_op);
		pcb* pcb = recibir_pcb(socket_cpu);
		loggear_pcb(pcb);

		push_cola_exit(pcb);

		sem_post(&sem_cpu_libre);
		sem_post(&sem_grado_multiprog);		
		break;		
	case -1:
		log_error(logger, "el cliente se desconecto. Terminando servidor");
	default:
		log_warning(logger,"Operacion desconocida. No quieras meter la pata");
		break;
	}
}