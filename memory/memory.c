#include "include/memory.h"

t_list *procesos_en_memoria;

int main(){
    logger = iniciar_logger("memory.log", "MEMORY");
    log_info(logger, "Logger de MEMORY iniciado");
    
    inicializar_configuracion();

    int server_fd = iniciar_servidor("MEMORY", config.ip_memoria, config.puerto_memoria);

	// Inicializo la lista de procesos en memoria
	procesos_en_memoria = list_create();

	esperar_handshake_cpu(server_fd);
    esperar_handshake_kernel(server_fd);

    crear_hilo_solicitudes_cpu();    
    crear_hilo_solicitudes_kernel();

    terminar_programa();

    return EXIT_SUCCESS;
}

void crear_hilo_solicitudes_kernel(){
	int err = pthread_create(&(hilo_kernel), NULL, gestionar_solicitudes_kernel, NULL);
    if (err != 0){
    	log_info(logger, "Hubo un problema al crear el thread de la escucha del servidor [%s]", strerror(err));
    }
	else{
    	log_info(logger, "El thread de la escucha del servidor inició su ejecución");
		pthread_join(hilo_kernel, NULL);
	}
}

void* gestionar_solicitudes_kernel(){
    t_list* lista;
    log_info(logger, "Esperando recibir operacion del kernel...");
	while (1) {
		int cod_op = recibir_operacion(socket_kernel);
        log_info(logger, "==============================================");
        log_info(logger, "Codigo recibido desde el kernel: %d", cod_op);

		switch (cod_op) {
		case CREAR_PROCESO_EN_MEMORIA:
            log_info(logger, "CREAR_PROCESO_EN_MEMORIA recibido.");

            solicitud_crear_proceso solicitud_crear_proceso = recibir_solicitud_crear_proceso(socket_kernel);
			pcb* pcb = solicitud_crear_proceso.pcb;

			crear_instr_set(solicitud_crear_proceso.filePath, pcb->pid);
            pcb->registros->ax = 1;
			
            // Operaciones crear proceso en memoria
            enviar_pcb(pcb, socket_kernel, CREAR_PROCESO_EN_MEMORIA);
			free(pcb);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
		
			terminar_programa();
            break;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");

            terminar_programa();
			break;
		}
        log_info(logger, "==============================================");
	}
}

void iniciar_gestor_solicitudes_en_hilo(pthread_t hilo, void* funcion_gestor){
	int err = pthread_create(&(hilo), NULL, funcion_gestor, NULL);
    if (err != 0){
    	log_info(logger, "Hubo un problema al crear el thread de la escucha del servidor [%s]", strerror(err));
    }
	else{
    	log_info(logger, "El thread de la escucha del servidor inició su ejecución");
		pthread_detach(hilo);
	}
}

void crear_hilo_solicitudes_cpu(){
	int err = pthread_create(&(hilo_cpu), NULL, gestionar_solicitudes_cpu, NULL);
    if (err != 0){
    	log_info(logger, "Hubo un problema al crear el thread de la escucha del servidor [%s]", strerror(err));
    }
	else{
    	log_info(logger, "El thread de la escucha del servidor inició su ejecución");
        pthread_detach(hilo_cpu);		
	}
}

void* gestionar_solicitudes_cpu(){
    t_list* lista;

    log_info(logger, "Esperando recibir operacion del CPU...");
	while (1) {    
		int cod_op = recibir_operacion(socket_cpu);
        log_info(logger, "==============================================");
        log_info(logger, "Codigo recibido desde el CPU: %d", cod_op);
		switch (cod_op) {
		case FETCH_INSTRUCCION:
            log_info(logger, "FETCH_INSTRUCCION recibido. ");

			recibir_mensaje(socket_cpu);
            // Operaciones de obtencion de instrucciones
            enviar_mensaje(FETCH_INSTRUCCION, "Respuesta FETCH_INSTRUCCION de memoria", socket_cpu);

			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
		
			terminar_programa();
            break;        
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
            terminar_programa();
			break;
		}
        log_info(logger, "==============================================");
	}
}

void terminar_programa()
{
    log_info(logger, "Memoria liberada correctamente");
    log_destroy(logger);
}


/**

Esta parte de la memoria será la encargada de obtener de los archivos de pseudo código las instrucciones y 
de devolverlas a pedido a la CPU.
Al momento de recibir la creación de un proceso, la memoria de instrucciones deberá leer el archivo de pseudocódigo indicado y
generar las estructuras que el grupo considere necesarias para poder devolver las instrucciones de a 1 a la CPU 
según ésta se las solicite por medio del Program Counter.
Ante cada petición se deberá esperar un tiempo determinado a modo de retardo en la obtención de la instrucción, y este tiempo,
 estará indicado en el archivo de configuración.


**/