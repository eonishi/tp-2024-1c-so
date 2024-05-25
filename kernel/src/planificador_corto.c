#include "../include/planificador_corto.h"
#include "../../cpu/include/checkInterrupt.h"

int inicio_quantum = 0;
pthread_mutex_t mutex_quantum = PTHREAD_MUTEX_INITIALIZER;

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

			solicitud_bloqueo_por_io solicitud = recibir_solicitud_bloqueo_por_io(socket_cpu_dispatch);
			solicitud.pcb->estado = BLOCKED;

			
			loggear_pcb(solicitud.pcb);			

			// Validacion IO valida
			// - No -> EXIT

			push_cola_blocked(solicitud.pcb);

            log_info(logger, "Voy a enviar [%s]", solicitud.instruc_io_tokenizadas[0]);
            log_info(logger, "Voy a enviar [%s]", solicitud.instruc_io_tokenizadas[1]);
            log_info(logger, "Voy a enviar [%s]", solicitud.instruc_io_tokenizadas[2]);
            
			enviar_instruccion_io(solicitud.instruc_io_tokenizadas,socket_io);
			sem_post(&sem_cpu_libre);		
			break;
        case INTERRUPCION:
            log_info(logger, "Recibi INTERRUPCION. CODIGO: %d", cod_op);

            pcb = recibir_pcb(socket_cpu_dispatch);
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
            log_info(logger, "inicio_quantum %d", inicio_quantum);
            inicio_quantum = 1; //con esto activo el contador de quantum
            log_info(logger, "inicio_quantum %d", inicio_quantum);
            pthread_mutex_unlock(&mutex_quantum);
            sem_wait(&sem_cpu_libre);
            //en este punto corren en paralelo la espera de quantum y el bloqueo de este semaforo
            //Si cumple el quantum se hace interrupt, nos devuelven el pcb en ejecucion y se libera el semaforo
			pthread_cancel((unsigned)monitoreo_quantum);

            log_info(logger, "CortoRR: Cpu libre! pasando proximo proceso a execute..");	
            pcb* pcb = pop_cola_ready();
            pcb->estado = EXECUTE;

            dispatch_proceso_planificador(pcb);

            gestionar_respuesta_cpu();
        }
    }
}

void *monitoreo_quantum(){
    log_info(logger,"Inicio conteo quantum");
    while(1){
        pthread_mutex_lock(&mutex_quantum);
        while(inicio_quantum){
            log_info(logger, "QUANTUM: %d", config->quantum);
            pthread_mutex_unlock(&mutex_quantum);
            sleep(2);
            log_info(logger, "hago send interrupt");
            //send_interrupt();
            //pthread_mutex_lock(&mutex_quantum);
            inicio_quantum = 0;
        }
    }
}

void send_interrupt(){
    log_info(logger, "hago send interrupt");
    //sem_post(&hilo_quantum);
    /*checkInterrupt(); //la funcion real va por checkInterrupt que nos devuelve un pcb?
    gestionar_respuesta_cpu();*/
}