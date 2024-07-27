#include "../include/planificador_corto.h"
#include "../../cpu/include/checkInterrupt.h"


int64_t q_usado, q_restante = 0;
t_temporal *q_transcurrido;
pthread_mutex_t mutex_quantum = PTHREAD_MUTEX_INITIALIZER;
pthread_t hilo_quantum;

void *iniciar_planificacion_corto(){
    while(1){

        log_info(logger, "Corto: Esperando otro proceso en ready");					
        sem_wait(&sem_proceso_en_ready);  			
		log_info(logger, "Corto: Llegó proceso en ready");		
		log_info(logger, "Corto: Esperando que el cpu este libre...");	
		sem_wait(&sem_cpu_libre);
		log_info(logger, "Corto: Cpu libre! pasando proximo proceso a execute..");	

        esperar_planificacion();

        pcb* pcb = pop_cola_ready();
        push_cola_execute(pcb); 

        dispatch_proceso_planificador(pcb);

        gestionar_respuesta_cpu();
        
    }
}


void dispatch_proceso_planificador(pcb* newPcb){
    enviar_pcb(newPcb, socket_cpu_dispatch, DISPATCH_PROCESO);

    if (strcmp(config->algoritmo_planificacion, "FIFO") != 0){
        q_transcurrido = temporal_create();
    }//aca creo el contador de quantum usado para RR/VRR
	log_info(logger, "Solicitud DISPATCH_PROCESO enviada a CPU");    
}

void gestionar_respuesta_cpu(){
    
	int cod_op = recibir_operacion(socket_cpu_dispatch);
	log_info(logger, "Codigo recibido desde el cpu: [%d]", cod_op);

	pcb* pcb;

	switch (cod_op) {
		case PROCESO_TERMINADO:
            cancelar_hilo_quantum();
			log_info(logger, "Recibi PROCESO_TERMINADO. CODIGO: %s", traduce_cod_op(cod_op));
			pcb = recibir_pcb(socket_cpu_dispatch);
			loggear_pcb(pcb);

            esperar_planificacion();
            pop_and_destroy_execute();

            log_info(logger_oblig, "Finaliza el proceso <%d> - Motivo: SUCCESS", pcb->pid); // validar log minimo
            push_cola_exit(pcb);

			sem_post(&sem_cpu_libre);
			break;		

		case PROCESO_BLOQUEADO_IO:
            log_info(logger, "Recibi PROCESO_BLOQUEADO. CODIGO: %s", traduce_cod_op(cod_op));

            cancelar_hilo_quantum();

            pcb = recibir_pcb(socket_cpu_dispatch);
            loggear_pcb(pcb);
            log_info(logger,"PID: %d - Bloqueado por: %s", pcb->pid, pcb->solicitud->instruc_io_tokenizadas[1]);

            esperar_planificacion();
            pop_and_destroy_execute();

            if(strcmp(config->algoritmo_planificacion,"FIFO") == 0){
                if(!validar_instruccion_a_io(pcb->solicitud->instruc_io_tokenizadas, pcb)){
                    log_info(logger_oblig, "Finaliza el proceso <%d> - Motivo: INVALID_INTERFACE", pcb->pid);

                    push_cola_exit(pcb);
                    sem_post(&sem_cpu_libre);
                    break;
                }

                enviar_proceso_a_esperar_io(pcb);

                log_info(logger_oblig, "PID: <%d> - Bloqueado por: <%s>", pcb->pid, pcb->solicitud->instruc_io_tokenizadas[1]);

                sem_post(&sem_cpu_libre);

                break;
            }
            
            else{//estoy usando RR o VRR

                if(q_usado > pcb->quantum) 
                    pcb->quantum = config->quantum;
                else
                    pcb->quantum -= q_usado;
                
                loggear_pcb(pcb);

                if(!validar_instruccion_a_io(pcb->solicitud->instruc_io_tokenizadas, pcb)){
                    log_info(logger_oblig, "Finaliza el proceso <%d> - Motivo: INVALID_INTERFACE", pcb->pid);

                    push_cola_exit(pcb);
                    sem_post(&sem_cpu_libre);
                    break;
                }

                enviar_proceso_a_esperar_io(pcb);

                log_info(logger_oblig, "PID: <%d> - Bloqueado por: <%s>", pcb->pid, pcb->solicitud->instruc_io_tokenizadas[1]);

                sem_post(&sem_cpu_libre);

                break;
            }
      
        case INTERRUPCION_USUARIO:
            pcb = recibir_pcb(socket_cpu_dispatch);
            loggear_pcb(pcb);
            log_info(logger_oblig, "Finaliza el proceso <%d> - Motivo: INTERRUPTED_BY_USER", pcb->pid);

            esperar_planificacion();
            // Gestion del proceso en las colas y su sincronizacion
            pop_and_destroy_execute();
            push_cola_exit(pcb);
            sem_post(&sem_cpu_libre);

            break;

        case INTERRUPCION_QUANTUM:
            pcb = recibir_pcb(socket_cpu_dispatch);
            loggear_pcb(pcb);
            
            log_info(logger_oblig, "PID <%d> - Desalojado por fin de Quantum.", pcb->pid);
            pcb->quantum = config->quantum;// por interrupcion se consumio todo el quantum del CPU

            esperar_planificacion();
            // Gestion del proceso en las colas y su sincronizacion
            pop_and_destroy_execute();
            push_cola_ready(pcb);
            sem_post(&sem_cpu_libre);

            break;

        case ERROR_DE_PROCESAMIENTO:
            log_error(logger, "Recibi ERROR_DE_PROCESAMIENTO. CODIGO: %d", cod_op);

            pcb = recibir_pcb(socket_cpu_dispatch);
            log_info(logger_oblig, "Finaliza el proceso <%d> - Motivo: INTERNAL_ERROR", pcb->pid); //validar log minimo

            esperar_planificacion();

            // Gestion del proceso en colas
            pop_and_destroy_execute();
            push_cola_exit(pcb);

            // Sincronizacion de ejecucion
            sem_post(&sem_cpu_libre);

            break;

        case PROCESO_SOLICITA_RECURSO:
            log_info(logger, "Recibi PROCESO_BLOQUEADO_RECURSO. CODIGO: %d", cod_op);

            pcb = recibir_pcb(socket_cpu_dispatch);
            char* nombre_recurso_solicitado = pcb->solicitud->instruc_io_tokenizadas[1];
            log_info(logger, "Proceso [%d] solicita recurso [%s]", pcb->pid, nombre_recurso_solicitado);

            esperar_planificacion();

            // Chequeo la existencia del recurso
            if(!recurso_existe(nombre_recurso_solicitado)){
                log_error(logger, "El recurso [%s] no existe", nombre_recurso_solicitado);
                log_info(logger_oblig, "Finaliza el proceso <%d> - Motivo: INVALID_RESOURCE", pcb->pid);
                pop_and_destroy_execute();
                push_cola_exit(pcb);
                sem_post(&sem_cpu_libre);
                break;
            }

            // Obtengo el recurso
            t_recurso* recurso_solicitado = get_recurso(nombre_recurso_solicitado);

            // Dependiendo si existen intancias disponibles, bloqueo o vuelvo a CPU
            if(recurso_disponible(recurso_solicitado)){
                wait_recurso(recurso_solicitado, pcb->pid);
                enviar_pcb(pcb, socket_cpu_dispatch, DISPATCH_PROCESO);

                // Lo hago recursivo porque sino tendria que separar la gestion de respuesta en un hilo
                // y la admicion de proceso a execute en otro hilo
                // TODO: probar si esto anda en condiciones
                gestionar_respuesta_cpu(); 
            }
            else {
                esperar_recurso(recurso_solicitado, pcb);
                log_info(logger_oblig, "PID: <%d> - Bloqueado por: <%s>", pcb->pid, pcb->solicitud->instruc_io_tokenizadas[1]);
                sem_post(&sem_cpu_libre);
            }

            break;

        case PROCESO_LIBERA_RECURSO:
            log_info(logger, "Recibi PROCESO_LIBERA_RECURSO. CODIGO: %d", cod_op);

            pcb = recibir_pcb(socket_cpu_dispatch);
            char* nombre_recurso_liberar = pcb->solicitud->instruc_io_tokenizadas[1];
            log_info(logger, "Proceso [%d] libera recurso [%s]", pcb->pid, nombre_recurso_liberar);

            esperar_planificacion();

            // Chequeo la existencia del recurso
            if(!recurso_existe(nombre_recurso_liberar)){
                log_error(logger, "El recurso [%s] no existe", nombre_recurso_liberar);
                log_info(logger_oblig, "Finaliza el proceso <%d> - Motivo: INVALID_RESOURCE", pcb->pid);
                pop_and_destroy_execute();
                push_cola_exit(pcb);
                sem_post(&sem_cpu_libre);
                break;
            }

            // Obtengo el recurso, lo libero y vuelvo a CPU
            t_recurso* recurso_liberar = get_recurso(nombre_recurso_liberar);
            signal_recurso(recurso_liberar, pcb->pid);
            enviar_pcb(pcb, socket_cpu_dispatch, DISPATCH_PROCESO);
            gestionar_respuesta_cpu();
            break;

        case OUT_OF_MEMORY:

            pcb = recibir_pcb(socket_cpu_dispatch);
            log_info(logger_oblig, "Finaliza el proceso <%d> - Motivo: OUT_OF_MEMORY", pcb->pid);

            esperar_planificacion();

                // Gestion del proceso en colas
            pop_and_destroy_execute();
            push_cola_exit(pcb);

                // Sincronizacion de ejecucion
            sem_post(&sem_cpu_libre);

            break;
      
		case -1:
            pcb = recibir_pcb(socket_cpu_dispatch);
			log_error(logger, "el cliente se desconecto. Terminando servidor");
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
	}
}

void *iniciar_planificacion_corto_RR(){
    q_usado = 0;
    while(1){
		log_info(logger, "CortoRR: Esperando otro proceso en ready");					
        sem_wait(&sem_proceso_en_ready);  			
		log_info(logger, "CortoRR: Llegó proceso en ready");		
		log_info(logger, "CortoRR: Esperando que el cpu este libre");	
        sem_wait(&sem_cpu_libre);
        log_info(logger, "CortoRR: Cpu libre! pasando proximo proceso a execute..");	

        esperar_planificacion();   

        pcb* pcb = pop_cola_ready();
        push_cola_execute(pcb);

        dispatch_proceso_planificador(pcb);
        crear_hilo_quantum(pcb->quantum);
        gestionar_respuesta_cpu();
        
    }
}

void *iniciar_planificacion_corto_VRR(){
    q_usado = 0;
    pcb* pcb = NULL;
    
    while(1){

		log_info(logger, "Corto VRR: Esperando otro proceso en ready");					
        sem_wait(&sem_proceso_en_ready);  			
		log_info(logger, "Corto VRR: Llegó proceso en ready");		
		log_info(logger, "Corto VRR: Esperando que el cpu este libre");	
        sem_wait(&sem_cpu_libre);
        log_info(logger, "Corto VRR: Cpu libre! pasando proximo proceso a execute..");

        esperar_planificacion();  

        if(!queue_is_empty(cola_readyVRR)){
            //dar paso a readyVRR prioritario
            pcb = pop_cola_ready_priority();
        }
        else{
            //dar paso a la cola ready normal
            q_usado = 0; //por que si hubo ejecucion de cola prioritaria queda seteado el valor de ese proceso
            log_info(logger, "La cola prioritaria está vacia.");
            pcb = pop_cola_ready();
        } 
        
        push_cola_execute(pcb); 
        dispatch_proceso_planificador(pcb);

        crear_hilo_quantum(pcb->quantum);
        gestionar_respuesta_cpu();
        
    }
}

void crear_hilo_quantum(unsigned quantum){
    int error = pthread_create(&hilo_quantum, NULL, monitoreo_quantum, (void*) quantum);
    if(error != 0){
        log_error(logger, "Hubo un error al crear el hilo de quatum");
    }
    pthread_detach(hilo_quantum);
}

void *monitoreo_quantum(void* quantum){
    unsigned q_restante = (unsigned) quantum;
    log_info(logger, "Inicio quantum de %d", q_restante);
    usleep((q_restante)*1000);
    interrumpir_proceso_ejecutando(INTERRUPCION_QUANTUM);
}

void cancelar_hilo_quantum(){
    if(strcmp(config->algoritmo_planificacion, "FIFO") != 0){
        pthread_cancel(hilo_quantum);
        temporal_stop(q_transcurrido); //detengo el contador de quantum usado
        q_usado = temporal_gettime(q_transcurrido); //lo casteo a milisegundos
        log_info(logger, "hilo quantum cancelado.");
    }
}
