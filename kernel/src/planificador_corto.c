#include "../include/planificador_corto.h"
#include "../../cpu/include/checkInterrupt.h"


int q_usado, q_restante = 0;
t_temporal *q_transcurrido;
pthread_mutex_t mutex_quantum = PTHREAD_MUTEX_INITIALIZER;
pthread_t hilo_quantum;

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
    if (strcmp(config->algoritmo_planificacion, "FIFO") != 0){
        q_transcurrido = temporal_create();
    }//aca creo el contador de quantum usado para VRR
    push_cola_execute(newPcb); 
	log_info(logger, "Solicitud DISPATCH_PROCESO enviada a CPU");    
}

void gestionar_respuesta_cpu(){
	t_list* lista;
    
	int cod_op = recibir_operacion(socket_cpu_dispatch);
	log_info(logger, "Codigo recibido desde el cpu: [%d]", cod_op);

	pcb* pcb;

	switch (cod_op) {
		case PROCESO_TERMINADO:
            cancelar_hilo_quantum();
			log_info(logger, "Recibi PROCESO_TERMINADO. CODIGO: %d", cod_op);
			pcb = recibir_pcb(socket_cpu_dispatch);
			loggear_pcb(pcb);
            pop_cola_execute();
			push_cola_exit(pcb);

			sem_post(&sem_cpu_libre);
			sem_post(&sem_grado_multiprog);
            		
			break;		
		case PROCESO_BLOQUEADO:
            cancelar_hilo_quantum();
            if(strcmp(config->algoritmo_planificacion,"FIFO") == 0){
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
            }else{//estoy usando RR o VRR
			temporal_stop(q_transcurrido); //detengo el contador de quantum usado
            q_usado = temporal_gettime(q_transcurrido); //lo casteo a milisegundos
            log_info(logger, "Recibi PROCESO_BLOQUEADO. CODIGO: %d", cod_op);
			solicitud_bloqueo_por_io solicitud = recibir_solicitud_bloqueo_por_io(socket_cpu_dispatch);
			solicitud.pcb->estado = BLOCKED;
			solicitud.pcb->quantum -= q_usado;//////////no calcule el q_usado en este punto
            loggear_pcb(solicitud.pcb);			
            push_cola_blocked(solicitud.pcb);
            log_info(logger, "Tokens de instr: [%s][%s][%s]", solicitud.instruc_io_tokenizadas[0],solicitud.instruc_io_tokenizadas[1], solicitud.instruc_io_tokenizadas[2]);
            bool enviado = validar_y_enviar_instruccion_a_io(solicitud.instruc_io_tokenizadas);
            if(!enviado)
                log_error(logger, "Hubo un error al intentar enviar las instrucciones a IO");
            sem_post(&sem_cpu_libre);
			break;
            }
        case INTERRUPCION:
            temporal_stop(q_transcurrido); //detengo el contador de quantum usado
            q_usado = temporal_gettime(q_transcurrido); //lo casteo a milisegundos
            pcb = recibir_pcb(socket_cpu_dispatch);
            log_info(logger, "Recibi proceso PID [%d] desalojado por INTERRUPCION CODIGO: [%d]", pcb->pid, cod_op);
            pcb->quantum -= config->quantum;// por interrupcion se consumio todo el quantum del CPU
            pcb->estado = READY;
            loggear_pcb(pcb);
            pop_cola_execute();
            push_cola_ready(pcb);
            sem_post(&sem_cpu_libre);
            sem_post(&sem_proceso_en_ready);		
            break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
	}
}

void *iniciar_planificacion_corto_RR(){
    q_usado = 0;
    while(1){
        if(planificacion_activada){   
			log_info(logger, "CortoRR: Esperando otro proceso en ready");					
            sem_wait(&sem_proceso_en_ready);  			
			log_info(logger, "CortoRR: Llegó proceso en ready");		
			log_info(logger, "CortoRR: Esperando que el cpu este libre");	
            sem_wait(&sem_cpu_libre);
            log_info(logger, "CortoRR: Cpu libre! pasando proximo proceso a execute..");	
            pcb* pcb = pop_cola_ready();
            pcb->estado = EXECUTE;
            dispatch_proceso_planificador(pcb);
            crear_hilo_quantum();
            gestionar_respuesta_cpu();
        }
    }
}

void *iniciar_planificacion_corto_VRR(){
    q_usado = 0;
    pcb* pcb = NULL;
    
    while(1){
        if(planificacion_activada){   
			log_info(logger, "Corto VRR: Esperando otro proceso en ready");					
            sem_wait(&sem_proceso_en_ready);  			
			log_info(logger, "Corto VRR: Llegó proceso en ready");		
			log_info(logger, "Corto VRR: Esperando que el cpu este libre");	
            sem_wait(&sem_cpu_libre);
            log_info(logger, "Corto VRR: Cpu libre! pasando proximo proceso a execute..");	
            if(!queue_is_empty(cola_readyVRR)){
                //dar paso a ready normal
                log_info(logger, "La cola prioritaria NO esta vacia.");
                 elemVRR* auxVRR = queue_pop(cola_readyVRR);
                 pcb = auxVRR->pcbVRR;
                 q_usado = auxVRR->quantum_usado;
            }else{
                //dar paso a la cola ready VRR
                log_info(logger, "La cola prioritaria está vacia.");
                pcb = pop_cola_ready();
            } 
            pcb->estado = EXECUTE;
            dispatch_proceso_planificador(pcb);
            crear_hilo_quantum();
            gestionar_respuesta_cpu();
        }
    }
}

void crear_hilo_quantum(){
    int error = pthread_create(&hilo_quantum, NULL, monitoreo_quantum, NULL);
    if(error != 0){
        log_error(logger, "Hubo un error al crear el hilo de quatum");
    }
    pthread_detach(hilo_quantum);
}

void *monitoreo_quantum(){
    q_restante = config->quantum - q_usado; // Si es FIFO no se trabaja con quantum.
    log_info(logger, "Inicio quantum de %d", q_restante);
    usleep((q_restante)*1000);
    send_interrupt();    
}

void send_interrupt(){
    pcb* pcb_en_cpu = queue_peek(cola_execute);
    enviar_interrupcion(socket_cpu_interrupt, pcb_en_cpu->pid); 
	log_info(logger, "Solicitud INTERRUPCION a PID %d enviada a CPU", pcb_en_cpu->pid);
}

void enviar_interrupcion(int socket_cliente, unsigned pid_enviado){
    size_t size;
    void* buffer_int = serializar_interrupcion(pid_enviado, &size);
    int bytes_enviados = send(socket_cliente, buffer_int,size ,0);
    free(buffer_int);
}

void* serializar_interrupcion(unsigned int valor, size_t* size) {
    *size = sizeof(unsigned int);
    void* buffer = malloc(*size);
    memcpy(buffer, &valor, *size);
    return buffer;
}

void cancelar_hilo_quantum(){
    if(strcmp(config->algoritmo_planificacion, "FIFO") != 0){
                pthread_cancel(hilo_quantum);
                log_info(logger, "hilo quantum cancelado.");
            }
}
