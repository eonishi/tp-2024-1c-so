#include "../include/planificador_corto.h"
#include "../../cpu/include/checkInterrupt.h"

int inicio_quantum = 0;
pthread_mutex_t mutex_quantum = PTHREAD_MUTEX_INITIALIZER;
pcb* unPCBPRUEBA; 

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

            push_cola_blocked(solicitud.pcb);

            log_info(logger, "Tokens de instr: [%s][%s][%s]", solicitud.instruc_io_tokenizadas[0],solicitud.instruc_io_tokenizadas[1], solicitud.instruc_io_tokenizadas[2]);

            bool enviado = validar_y_enviar_instruccion_a_io(solicitud.instruc_io_tokenizadas);

            if(!enviado)
                log_error(logger, "Hubo un error al intentar enviar las instrucciones a IO");

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
			//pthread_cancel(monitoreo_quantum);

            log_info(logger, "CortoRR: Cpu libre! pasando proximo proceso a execute..");	
            pcb* pcb = pop_cola_ready();
            pcb->estado = EXECUTE;
            unPCBPRUEBA = pcb;

            dispatch_proceso_planificador(pcb);

            gestionar_respuesta_cpu();
        }
    }
}

void *monitoreo_quantum(){
    log_info(logger,"Inicio conteo quantum");
    pthread_mutex_lock(&mutex_quantum);
    while(1){
        pthread_mutex_lock(&mutex_quantum);
        sleep(5);
        send_interrupt();
    }
}

void send_interrupt(){
    log_info(logger, "hago send interrupt");
    pcb* pcb_en_cpu = unPCBPRUEBA;//queue_peek(cola_ready);
    log_info(logger, "TENGO EL PCB DE READY");
    enviar_interrupcion(socket_cpu_interrupt, pcb_en_cpu->pid); 
	log_info(logger, "Solicitud INTERRUPCION enviada a CPU");
    gestionar_respuesta_cpu();
}

void enviar_interrupcion(int socket_cliente, unsigned pid_enviado){
    size_t size;
    void* buffer = serializar_interrupcion(pid_enviado, &size);
    int bytes_enviados = send(socket_cliente, buffer,size ,0);
    free(buffer);
}

void* serializar_interrupcion(unsigned int valor, size_t* size) {
    *size = sizeof(unsigned int);
    void* buffer = malloc(*size);
    memcpy(buffer, &valor, *size);
    return buffer;
}

