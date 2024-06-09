#include "include/memory.h"

t_list *procesos_en_memoria;

int main(){
    logger = iniciar_logger("memory.log", "MEMORY");
    log_info(logger, "Logger de MEMORY iniciado");
    
    inicializar_configuracion();

    int server_fd = iniciar_servidor("MEMORY", config.ip_memoria, config.puerto_memoria);

	// Inicializo la lista de procesos en memoria
	procesos_en_memoria = list_create();

	inicializar_memoria();
	int cantidad = 64;
	int un_numero = 120;
	if (tengo_espacio_para_agregar(cantidad))
	{
		void* data = malloc(11);
		strcpy(data, "Hola mundo");
		//for(size_t i = 0; i < cantidad; i++){
		//	set_frame(i, 2, data);
		//}
		//imprimir_data(0, 2);
		//imprimir_data(1, 0);
		//imprimir_data(1, 1);
		//imprimir_data(1, 2);
		//imprimir_data(4, 2);
		//imprimir_data(12, 2);

		set_frame(0, 0, data, strlen(data) + 1);
		imprimir_data(0,0, strlen(data) + 1);

		set_frame(100, 0, &un_numero, sizeof(un_numero));
		imprimir_data(100,0, sizeof(int));

		void *test = malloc(5);
		strcpy(test, "test");
		set_frame(101, 0, test, strlen(test) + 1);
		imprimir_data(101, 0, strlen(test) + 1);

		//log_info(logger, "imprimo todos los frames:");
		//imprimir_frames();
	}
	else
	{
		log_info(logger, "No tengo espacio para agregar [%d] frames", cantidad);
	}

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

			// (SUS) podria recibir solo el path y PID
            solicitud_crear_proceso solicitud_crear_proceso = recibir_solicitud_crear_proceso(socket_kernel);
			pcb* pcb = solicitud_crear_proceso.pcb;

			cargar_proceso_en_memoria(solicitud_crear_proceso.filePath, pcb->pid);
			
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

    log_info(logger, "Esperando recibir operacion del CPU...");
	while (1) {    
		int cod_op = recibir_operacion(socket_cpu);
        log_info(logger, "==============================================");
        log_info(logger, "Codigo recibido desde el CPU: %d", cod_op);
		switch (cod_op) {
		case FETCH_INSTRUCCION:
            log_info(logger, "FETCH_INSTRUCCION recibido. ");

			recibir_fetch_de_cpu();
			esperar_retardo();
			enviar_instruccion_a_cpu();		
			break;
		case REDIMENSIONAR_MEMORIA_PROCESO:
			log_info(logger, "REDIMENSIONAR_MEMORIA_PROCESO recibido.");

			unsigned cantidad_resize = recibir_cantidad(socket_cpu);
			log_info(logger, "Cantidad de paginas a redimensionar: %d", cantidad_resize);
			// TODO: cambio la cantidad entradas de la tabla de paginas del proceso
			// TODO: Checkear si el resize fue exitoso o no y enviar respuesta
			enviar_status(SUCCESS, socket_cpu);
			log_info(logger, "Se redimensionó la memoria correctamente, envio: [%d]", SUCCESS);
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