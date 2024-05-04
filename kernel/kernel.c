#include "include/kernel.h"


pthread_t tid[2];

int main(){
    logger = iniciar_logger("kernel.log", "KERNEL");
	log_info(logger, "Logger de Kernel iniciado");

	config = inicializar_config();

	//creo colas de estados
	t_list procesoNew;
	t_list procesoReady;
	t_list procesoExecute;
	t_list procesoBlock; //iniciamos con una, segun consigna va a haber mas (una por cada I/O)
	t_list procesoExit;

	if(!cargar_configuracion(config) || !generar_conexiones(&socket_cpu, &socket_memoria)){
		log_error(logger, "No se puede iniciar. Se cierra el programa");
		return EXIT_FAILURE;
	}

	iniciar_servidor_en_hilo();

	iniciar_consola();

	terminar_programa();

	return EXIT_SUCCESS;
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

		if (strcmp(leido, "") == 0)
			ingresoActivado = 0;
			
		if(strcmp(leido, "INICIAR_PROCESO") == 0){
			log_info(logger, "==============================================");
			log_info(logger, "Inicio de ejecución de INICIAR_PROCESO");
					
			pcb* pcb = iniciar_proceso_en_memoria(socket_memoria);

			//aca estoy RM
			if(strcmp(config->algoritmoPC,"FIFO")==0){
					planificadorFIFO(pcb);
				}else if (strcmp(config->algoritmoPC,"FIFO")==0){
					planificadorRR(pcb);
				}else if (strcmp(config->algoritmoPC,"FIFO")==0){
					planificadorVRR(pcb);
			}

			dispatch_proceso(pcb);
			log_info(logger, "Fin de ejecución de INICIAR_PROCESO");
			log_info(logger, "==============================================");

		}
		else if(strcmp(leido, "CPU") == 0){
			log_info(logger, "Enviando mensaje al CPU...");
    		enviar_mensaje(MENSAJE, "0", socket_cpu);
			log_info(logger, "Mensaje enviado");
		}
		else if(strcmp(leido, "MEMORIA") == 0) {
			log_info(logger, "Enviando mensaje a la MEMORIA...");
    		enviar_mensaje(MENSAJE,"0", socket_memoria);
			log_info(logger, "Mensaje enviado");
		}
		else{
			log_info(logger, "Comando desconocido");
		}

		free(leido);
	}
}

pcb* iniciar_proceso_en_memoria(){
	pcb* pcb = crear_pcb(pcb_counter++, 4000);

	pcb->registros->ax = 9;

	enviar_pcb(pcb, socket_memoria, CREAR_PROCESO_EN_MEMORIA);
	log_info(logger, "Solicitud CREAR_PROCESO_EN_MEMORIA enviada a memoria");
	
	pcb = esperar_pcb(socket_memoria, CREAR_PROCESO_EN_MEMORIA);

	log_info(logger, "Respuesta CREAR_PROCESO_EN_MEMORIA recibida");
	log_info(logger, "Ax es: [%d]", pcb->registros->ax);	
	
	return pcb;
}

void dispatch_proceso(pcb* new_pcb){
	enviar_pcb(new_pcb, socket_cpu, DISPATCH_PROCESO);
	log_info(logger, "Solicitud DISPATCH_PROCESO enviada a CPU");
	
	pcb* pcb_respuesta;
	pcb_respuesta = esperar_pcb(socket_cpu, DISPATCH_PROCESO);
	loggear_pcb(pcb_respuesta);

	log_info(logger, "Respuesta DISPATCH_PROCESO recibida");
}

void terminar_programa()
{
    liberar_conexion(socket_cpu);
    liberar_conexion(socket_memoria);

    log_info(logger, "Memoria liberada correctamente");
    log_destroy(logger);
}

void *iniciar_escucha(){
	int server_socket = iniciar_servidor("KERNEL", config->ip_kernel, config->puerto_kernel);
	int cliente_fd = esperar_cliente(server_socket);

	t_list* lista;
	bool on = 1;
	while (on) {
        log_info(logger, "Estoy por recibir operacion");
		int cod_op = recibir_operacion(cliente_fd);
        log_info(logger, "Codigo recibido:");

		switch (cod_op) {
		case MENSAJE:
            log_info(logger, "Entre a MENSAJE. CODIGO: %d", cod_op);
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

void iniciar_servidor_en_hilo(){
	int err = pthread_create(&(tid[0]), NULL, iniciar_escucha, NULL);
    if (err != 0){
    	log_info(logger, "Hubo un problema al crear el thread de la escucha del servidor [%s]", strerror(err));
    }
	else{
    	log_info(logger, "El thread de la escucha del servidor inició su ejecución");
		pthread_detach(tid[0]);
	}
}

void planificadorFIFO(pcb* pcb){
// Push pcb cola new etc //en proceso, falta ordenar para que el planificador tenga sentido
	list_add(procesoNew,pcb);
	int pidActual = pcb->pid;
	log_info(logger, "Se crea el proceso <%d> en NEW",pidActual); //log requerido por consigna
	//planificador L chequea grado multiprogramacion
	if (sizeof(procesoReady)<config->gradoMultiprogramacion){
		list_remove(procesoNew,0);//ajustar no va 0
		list_add(procesoReady,pcb);
	}
}

void planificadorRR(pcb* pcb){
	//TODO
}
void planificadorVRR(pcb* pcb){
	//TODO
}
/*
El módulo Kernel, en el contexto de nuestro trabajo práctico, será el encargado de gestionar la ejecución de los 
diferentes procesos que se generen por medio de su consola interactiva.

*/