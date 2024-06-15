#include "include/kernel.h"

int main(){
    logger = iniciar_logger("kernel.log", "KERNEL");
	log_info(logger, "Logger de Kernel iniciado");

	config = inicializar_config();

	if(!cargar_configuracion(config) || !generar_conexiones()){
		log_error(logger, "No se puede iniciar. Se cierra el programa");
		return EXIT_FAILURE;
	}

	log_info(logger, "Iniciando semaforos");
	iniciar_semaforos();
	inicializar_colas_planificador();

	log_info(logger, "Creando hilo para recibir conexiones io...");
	iniciar_hilo(esperar_y_escuchar_conexiones_io, hilo_conexiones_io);

	log_info(logger, "Creando hilo para el planificador de largo plazo...");
	iniciar_hilo(iniciar_planificacion_largo, hilo_planificador_largo);

	log_info(logger, "algortimo: %s", config->algoritmo_planificacion);
	if(strcmp(config->algoritmo_planificacion, "FIFO") == 0){
		log_info(logger, "Creando hilo para el planificador de corto plazo FIFO...");
		iniciar_hilo(iniciar_planificacion_corto, hilo_planificador_corto);
	}else if(strcmp(config->algoritmo_planificacion, "RR") == 0){
		log_info(logger, "Creando hilo para el planificador de corto plazo ROUND ROBIN...");
		iniciar_hilo(iniciar_planificacion_corto_RR, hilo_planificador_corto_RR);
		log_info(logger, "Creando hilo para el contador de quantum...");
		//iniciar_hilo(monitoreo_quantum, hilo_quantum); eliminar si las pruebas dan bien
	}else if(strcmp(config->algoritmo_planificacion, "VRR") == 0){ //SE DEBE MODIFICAR AL IMPLEMENTAR VRR
		log_info(logger, "Creando hilo para el planificador de corto plazo VIRTUAL ROUND ROBIN...");
		iniciar_hilo(iniciar_planificacion_corto, hilo_planificador_corto);
	}

	iniciar_consola();

	terminar_programa();

	return EXIT_SUCCESS;
}

void* esperar_y_escuchar_conexiones_io(){
    lista_conexiones_io = list_create();
	pthread_t io_threads[10]; // TODO Mejorar
	int io_thread_index = 0;

    while(1){
        conexion_io conexion_io = recibir_conexion_io(socket_server_kernel);

		list_add(lista_conexiones_io, &conexion_io);

		int socket = conexion_io.socket;

		iniciar_hilo_con_args(escuchar_io, io_threads[io_thread_index++], &socket);
    }
}


void iniciar_semaforos() {
	sem_init(&sem_nuevo_proceso, 0, 0);
	sem_init(&sem_proceso_en_ready, 0, 0);
	sem_init(&sem_cpu_libre, 0, 1);
	sem_init(&sem_grado_multiprog, 0, config->grado_multiprogramacion);
}

void iniciar_consola()
{
	char* leido;
	leido = "void";
	bool ingresoActivado = 1;

	while(ingresoActivado){
		log_info(logger, "Ingrese INICIAR_PROCESO:");

		leido = readline("> ");
		log_info(logger, "Linea ingresada: %s", leido);

		char** leido_split = string_n_split(leido, 2, " ");
		char* comando = leido_split[0];

		if (strcmp(comando, "") == 0)
			ingresoActivado = 0;
		
		if(strcmp(comando, "PROCESO_ESTADO") == 0){
			imprimir_colas();
		}
		else if(strcmp(comando, "INICIAR_PLANIFICACION") == 0){
			planificacion_activada = 1;
		}
		else if(strcmp(comando, "DETENER_PLANIFICACION") == 0){
			planificacion_activada = 0;
		}
		else if(strcmp(comando, "INICIAR_PROCESO") == 0){
			log_info(logger, "==============================================");
			log_info(logger, "Inicio de ejecución de INICIAR_PROCESO");

			char* filePath = leido_split[1];

			log_info(logger, "Con argumento: [%s]", filePath);
					
			pcb* pcb = iniciar_proceso_en_memoria(filePath);

			log_info(logger, "PCB creado. Id: [%d]", pcb->pid);

			push_cola_new(pcb);
			sem_post(&sem_nuevo_proceso);
      
			log_info(logger, "Fin de ejecución de INICIAR_PROCESO");
			log_info(logger, "==============================================");

		}
		else{
			log_info(logger, "Comando desconocido");
		}

		free(leido);
	}
}

pcb* iniciar_proceso_en_memoria(char* filePath){
	pcb* pcb = crear_pcb(pcb_counter++, 4000);

	enviar_solicitud_crear_proceso(filePath, pcb, socket_memoria);
	log_info(logger, "Solicitud CREAR_PROCESO_EN_MEMORIA enviada a memoria");
	
	pcb = esperar_pcb(socket_memoria, CREAR_PROCESO_EN_MEMORIA);

	log_info(logger, "Respuesta CREAR_PROCESO_EN_MEMORIA recibida");
	log_info(logger, "Ax es: [%d]", pcb->registros->ax);	
	
	return pcb;
}

void terminar_programa()
{
    liberar_conexion(socket_cpu_dispatch);
    liberar_conexion(socket_memoria);

    log_info(logger, "Memoria liberada correctamente");
    log_destroy(logger);
}

void *iniciar_escucha_servidor(void *socket){
	int cliente_fd = *((int*) socket);

	log_info(logger, "Socket io: [%d]", cliente_fd);

	t_list* lista;
	bool on = 1;
	while (on) {
        log_info(logger, "Estoy por recibir operacion");
		int cod_op = recibir_operacion(cliente_fd);
        log_info(logger, "Codigo recibido:");

		switch (cod_op) {
		case FIN_EJECUCION_IO:
			log_info(logger, "Recibi FIN_EJECUCION_IO. CODIGO: %d", cod_op);
			recibir_mensaje(cliente_fd);

			pcb* pcb_blocked = pop_cola_blocked();
			pcb_blocked->estado = READY;
			push_cola_ready(pcb_blocked);
			sem_post(&sem_proceso_en_ready);
			break;
		case DISPATCH_PROCESO:
            log_info(logger, "Recibi DISPATCH_PROCESO. CODIGO: %d", cod_op);
			pcb* pcb = recibir_pcb(cliente_fd);
			loggear_pcb(pcb);
			log_info(logger, "Respuesta DISPATCH_PROCESO recibida");
			break;		
		case MENSAJE:
            log_info(logger, "Recibi MENSAJE. CODIGO: %d", cod_op);
			recibir_mensaje(cliente_fd);
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

void iniciar_hilo(void* func, pthread_t thread){
	int err = pthread_create(&thread, NULL, func, NULL);

    if (err != 0){
    	log_info(logger, "Hubo un problema al crear el hilo. Error: [%s]", strerror(err));
    }
	else{
    	log_info(logger, "El hilo se inició su correctamente");
		pthread_detach(thread);
	}
}

void iniciar_hilo_con_args(void *(*func)(void *), pthread_t thread, void* args){
	int err = pthread_create(&thread, NULL, func, args);

    if (err != 0){
    	log_info(logger, "Hubo un problema al crear el hilo. Error: [%s]", strerror(err));
    }
	else{
    	log_info(logger, "El hilo se inició su correctamente");
		pthread_detach(thread);
	}
}