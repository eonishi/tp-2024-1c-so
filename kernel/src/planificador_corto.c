#include "../include/planificador_corto.h"
#include "../../cpu/include/checkInterrupt.h"


int q_usado, q_restante = 0;
t_temporal *q_transcurrido;
pthread_mutex_t mutex_quantum = PTHREAD_MUTEX_INITIALIZER;
pthread_t hilo_quantum;

void *iniciar_planificacion_corto(){
    
    while(1){// si se hace while(planificacion_activada) salta el while porque empieza en cero y luego no ejecuta
        if(planificacion_activada){
		log_info(logger, "Corto: Esperando otro proceso en ready");					
        sem_wait(&sem_proceso_en_ready);  			
		log_info(logger, "Corto: Llegó proceso en ready");		
		log_info(logger, "Corto: Esperando que el cpu este libre...");	
		sem_wait(&sem_cpu_libre);
		log_info(logger, "Corto: Cpu libre! pasando proximo proceso a execute..");	

        pcb* pcb = pop_cola_ready();
        push_cola_execute(pcb); 

        dispatch_proceso_planificador(pcb);

        gestionar_respuesta_cpu();
        }
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


            pop_and_destroy(cola_execute, (void*) destruir_pcb);
            log_info(logger, "Finaliza el proceso <%d> - Motivo: SUCCESS", pcb->pid); //validar log minimo

            push_cola_exit(pcb);

			sem_post(&sem_cpu_libre);
			sem_post(&sem_grado_multiprog);
            		
			break;		

		case PROCESO_BLOQUEADO_IO:
            log_info(logger, "Recibi PROCESO_BLOQUEADO. CODIGO: %s", traduce_cod_op(cod_op));

            cancelar_hilo_quantum();
            pop_and_destroy(cola_execute, (void*) destruir_pcb);

            pcb = recibir_pcb(socket_cpu_dispatch);
            loggear_pcb(pcb);
            log_info(logger,"PID: %d - Bloqueado por: %s", pcb->pid, pcb->solicitud->instruc_io_tokenizadas[1]);

            if(strcmp(config->algoritmo_planificacion,"FIFO") == 0){
                if(!validar_instruccion_a_io(pcb->solicitud->instruc_io_tokenizadas, pcb)){
                    log_error(logger, "Error de validación de instrucción a IO");
                    log_info(logger, "Fin de ejecución de proceso [%d]", pcb->pid);
                    push_cola_exit(pcb);
                    sem_post(&sem_cpu_libre);
                    break;
                }

                enviar_proceso_a_esperar_io(pcb);
                sem_post(&sem_cpu_libre);

                break;
            }
            
            else{//estoy usando RR o VRR
                temporal_stop(q_transcurrido); //detengo el contador de quantum usado
                q_usado = temporal_gettime(q_transcurrido); //lo casteo a milisegundos
                
                //solicitud.pcb->estado = BLOCKED; //lo hace la funcion push
                pcb->quantum -= q_usado;
                loggear_pcb(pcb);

                if(!validar_instruccion_a_io(pcb->solicitud->instruc_io_tokenizadas, pcb)){
                    log_error(logger, "Error de validación de instrucción a IO");
                    log_info(logger, "Fin de ejecución de proceso [%d]", pcb->pid);
                    push_cola_exit(pcb);
                    sem_post(&sem_cpu_libre);
                    break;
                }

                enviar_proceso_a_esperar_io(pcb);
                sem_post(&sem_cpu_libre);

                break;
            }
      
        case INTERRUPCION:
            pcb = recibir_pcb(socket_cpu_dispatch);
            loggear_pcb(pcb);

            // Existen interrupcion que no son por quantum
            // TODO: Gestionar casos donde la interrupcion no es por Q

            log_info(logger, "FIn de Quantum: PID %d desalojado por fin de Quantum.", pcb->pid);
            pcb->quantum -= config->quantum;// por interrupcion se consumio todo el quantum del CPU


            // Gestion del proceso en las colas y su sincronizacion
            pop_and_destroy(cola_execute, (void*) destruir_pcb);
            push_cola_ready(pcb);
            sem_post(&sem_cpu_libre);

            break;
            
        case ERROR_DE_PROCESAMIENTO:
            log_error(logger, "Recibi ERROR_DE_PROCESAMIENTO. CODIGO: %d", cod_op);
            log_info(logger, "Finaliza el proceso <%d> - Motivo: INVALID_INTERFACE", pcb->pid); //validar log minimo

            pcb = recibir_pcb(socket_cpu_dispatch);

            // Gestion del proceso en colas
            pop_and_destroy(cola_execute, (void*) destruir_pcb);
            push_cola_exit(pcb);

            // Sincronizacion de ejecucion
            sem_post(&sem_cpu_libre);
            sem_post(&sem_grado_multiprog);

            break;

        case PROCESO_SOLICITA_RECURSO:
            log_info(logger, "Recibi PROCESO_BLOQUEADO_RECURSO. CODIGO: %d", cod_op);

            pcb = recibir_pcb(socket_cpu_dispatch);
            char* nombre_recurso_solicitado = pcb->solicitud->instruc_io_tokenizadas[1];
            log_info(logger, "Proceso [%d] solicita recurso [%s]", pcb->pid, nombre_recurso_solicitado);

            // Chequeo la existencia del recurso
            if(!recurso_existe(nombre_recurso_solicitado)){
                log_error(logger, "El recurso [%s] no existe", nombre_recurso_solicitado);
                pop_and_destroy(cola_execute, (void*) destruir_pcb);
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
                sem_post(&sem_cpu_libre);
            }

            break;

        case PROCESO_LIBERA_RECURSO:
            log_info(logger, "Recibi PROCESO_LIBERA_RECURSO. CODIGO: %d", cod_op);

            pcb = recibir_pcb(socket_cpu_dispatch);
            char* nombre_recurso_liberar = pcb->solicitud->instruc_io_tokenizadas[1];
            log_info(logger, "Proceso [%d] libera recurso [%s]", pcb->pid, nombre_recurso_liberar);

            // Chequeo la existencia del recurso
            if(!recurso_existe(nombre_recurso_liberar)){
                log_error(logger, "El recurso [%s] no existe", nombre_recurso_liberar);
                pop_and_destroy(cola_execute, (void*) destruir_pcb);
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
        log_info(logger, "Finaliza el proceso <%d> - Motivo: OUT_OF_MEMORY", pcb->pid); //validar log minimo
        break;
      
		case -1:
            pcb = recibir_pcb(socket_cpu_dispatch);
			log_error(logger, "el cliente se desconecto. Terminando servidor");
            log_info(logger, "Finaliza el proceso <%d> - Motivo: INVALID_INTERFACE", pcb->pid); //validar log minimo
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
            push_cola_execute(pcb);
            //pcb->estado = EXECUTE; //// REVISAR SI HACE FALTA, LOS CAMBIOS DE ESTADO DEBEN MANEJARSE CON LOS PUSH
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
                //dar paso a readyVRR prioritario
                log_info(logger, "La cola prioritaria NO esta vacia.");
                 elemVRR* auxVRR = queue_pop(cola_readyVRR);
                 pcb = auxVRR->pcbVRR;
                 q_usado = auxVRR->quantum_usado; //aca defino el quantum que va a usar el proceso en la cola ready prioridad
            }else{
                //dar paso a la cola ready normal
                q_usado = 0; //por que si hubo ejecucion de cola prioritaria queda seteado el valor de ese proceso
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
    interrumpir_proceso_ejecutando();
}

void cancelar_hilo_quantum(){
    if(strcmp(config->algoritmo_planificacion, "FIFO") != 0){
                pthread_cancel(hilo_quantum);
                log_info(logger, "hilo quantum cancelado.");
            }
}

const char* traduce_cod_op(op_code code) {
    switch (code) {
        case MENSAJE: return "MENSAJE";
        case PAQUETE: return "PAQUETE";
        case HANDSHAKE: return "HANDSHAKE";
        case CREAR_PROCESO_EN_MEMORIA: return "CREAR_PROCESO_EN_MEMORIA";
        case LIBERAR_PROCESO_EN_MEMORIA: return "LIBERAR_PROCESO_EN_MEMORIA";
        case DISPATCH_PROCESO: return "DISPATCH_PROCESO";
        case FETCH_INSTRUCCION: return "FETCH_INSTRUCCION";
        case REDIMENSIONAR_MEMORIA_PROCESO: return "REDIMENSIONAR_MEMORIA_PROCESO";
        case ESCRIBIR_DATO_EN_MEMORIA: return "ESCRIBIR_DATO_EN_MEMORIA";
        case LEER_DATO_DE_MEMORIA: return "LEER_DATO_DE_MEMORIA";
        case DATO_LEIDO_DE_MEMORIA: return "DATO_LEIDO_DE_MEMORIA";
        case CONSULTAR_TABLA_DE_PAGINAS: return "CONSULTAR_TABLA_DE_PAGINAS";
        case RESPUESTA: return "RESPUESTA";
        case INTERRUPCION: return "INTERRUPCION";
        case PROCESO_TERMINADO: return "PROCESO_TERMINADO";
        case PROCESO_BLOQUEADO_IO: return "PROCESO_BLOQUEADO";
        case PROCESO_SOLICITA_RECURSO: return "PROCESO_SOLICITA_RECURSO";
        case PROCESO_LIBERA_RECURSO: return "PROCESO_LIBERA_RECURSO";
        case PROCESO_DESALOJADO: return "PROCESO_DESALOJADO";
        case ERROR_DE_PROCESAMIENTO: return "ERROR_DE_PROCESAMIENTO";
        case EJECUTAR_INSTRUCCION_IO: return "EJECUTAR_INSTRUCCION_IO";
        case FIN_EJECUCION_IO: return "FIN_EJECUCION_IO";
        case SUCCESS: return "SUCCESS";
        case OUT_OF_MEMORY: return "OUT_OF_MEMORY";
        default: return "UNKNOWN";
    }
}