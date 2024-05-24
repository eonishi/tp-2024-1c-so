#include "../include/planificador_corto.h"
#include "../../cpu/include/checkInterrupt.h"

//pthread_t hilo_quantum;
int inicio_quantum=0;

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
    enviar_pcb(newPcb, socket_cpu_dispatch, DISPATCH_PROCESO); 
	log_info(logger, "Solicitud DISPATCH_PROCESO enviada a CPU");    
}

void gestionar_respuesta_cpu(){
	t_list* lista;

	int cod_op = recibir_operacion(socket_cpu_dispatch);
	log_info(logger, "Codigo recibido desde el cpu: [%d]", cod_op);

	pcb* pcb;

	switch (cod_op) {
		case PROCESO_TERMINADO:
			log_info(logger, "Recibi PROCESO_TERMINADO. CODIGO: %d", cod_op);
			pcb = recibir_pcb(socket_cpu_dispatch);
			loggear_pcb(pcb);

			push_cola_exit(pcb);

			sem_post(&sem_cpu_libre);
			sem_post(&sem_grado_multiprog);		
			break;		
		case PROCESO_BLOQUEADO:
			log_info(logger, "Recibi PROCESO_BLOQUEADO. CODIGO: %d", cod_op);

			pcb = recibir_pcb(socket_cpu_dispatch);
			pcb->estado = BLOCKED;

			loggear_pcb(pcb);

			push_cola_blocked(pcb);

			sem_post(&sem_cpu_libre);		
			break;
        case INTERRUPCION:
            log_info(logger, "Recibi INTERRUPCION. CODIGO: %d", cod_op);

            pcb = recibir_pcb(socket_cpu);
            pcb->estado = READY;

            loggear_pcb(pcb);

            push_cola_ready(pcb);

            sem_post(&sem_cpu_libre);		
            break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
	}
}

void *iniciar_planificacion_corto_RR(){
    
    

    while(1){
        if(planificacion_activada){   
			log_info(logger, "CortoRR: Esperando otro proceso en ready");					
            sem_wait(&sem_proceso_en_ready);  			
			log_info(logger, "CortoRR: Llegó proceso en ready");		
			log_info(logger, "CortoRR: Esperando que el cpu este libre o se cumpla quantum...");	

            inicio_quantum = 1;
            log_info(logger, "CortoRR: VALOR INICIO_QUANTUM = %d",inicio_quantum);
            sem_wait(&sem_cpu_libre);
            //en este punto corren en paralelo la espera de quantum y el bloqueo de este semaforo
            //Si cumple el quantum se hace interrupt, nos devuelven el pcb en ejecucion y se libera el semaforo
			pthread_cancel(monitoreo_quantum);

            log_info(logger, "CortoRR: Cpu libre! pasando proximo proceso a execute..");	
            pcb* pcb = pop_cola_ready();
            pcb->estado = EXECUTE;

            dispatch_proceso_planificador(pcb);

            gestionar_respuesta_cpu();
        }
    }
}

void *monitoreo_quantum(){
    log_info(logger,"mi funcion monitoreo funciona");
    while(1){
        while(inicio_quantum){
                //sleep(2);
                log_info(logger, "hago interrupt por qantum");
                inicio_quantum = 0;
        }
    }
  //send_interrupt();
}

void send_interrupt(){
    log_info(logger, "hago send interrupt");
    //sem_post(&hilo_quantum);
    /*checkInterrupt(); //la funcion real va por checkInterrupt que nos devuelve un pcb?
    gestionar_respuesta_cpu();*/
}