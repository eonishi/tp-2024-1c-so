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

		char** leido_split = string_n_split(leido, 2, " ");
		char* comando = leido_split[0];

		if (strcmp(comando, "") == 0)
			ingresoActivado = 0;
			
		if(strcmp(comando, "INICIAR_PROCESO") == 0){
			log_info(logger, "==============================================");
			log_info(logger, "Inicio de ejecución de INICIAR_PROCESO");

			char* filePath = leido_split[1];

			log_info(logger, "Con argumento: [%s]", filePath);
					
			pcb* pcb = iniciar_proceso_en_memoria(filePath);

			//aca estoy RM
			// list_add(procesoNew,pcb); //agrego el pcb creado a cola de new
			// int pidActual = pcb->pid;
			// log_info(logger, "Se crea el proceso <%d> en NEW",pidActual); //log requerido por consigna
			// planificador();
			//redefino dispatch_proceso(pcb); y lo meto en el planificador
      
			log_info(logger, "Fin de ejecución de INICIAR_PROCESO");
			log_info(logger, "==============================================");

		}
		else if(strcmp(comando, "CPU") == 0){
			log_info(logger, "Enviando mensaje al CPU...");
    		enviar_mensaje(MENSAJE, "0", socket_cpu);
			log_info(logger, "Mensaje enviado");
		}
		else if(strcmp(comando, "MEMORIA") == 0) {
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

pcb* iniciar_proceso_en_memoria(char* filePath){
	pcb* pcb = crear_pcb(pcb_counter++, 4000);

	pcb->registros->ax = 9;

	enviar_solicitud_crear_proceso(filePath, pcb, socket_memoria);
	// enviar_pcb(pcb, socket_memoria, CREAR_PROCESO_EN_MEMORIA);
	log_info(logger, "Solicitud CREAR_PROCESO_EN_MEMORIA enviada a memoria");
	
	pcb = esperar_pcb(socket_memoria, CREAR_PROCESO_EN_MEMORIA);

	log_info(logger, "Respuesta CREAR_PROCESO_EN_MEMORIA recibida");
	log_info(logger, "Ax es: [%d]", pcb->registros->ax);	
	
	return pcb;
}
void dispatch_proceso(){
	if(list_size(procesoReady)>0){
		pcb* new_pcb = list_get(procesoReady,0); //obtengo el primer proceso de la lista ready
		
		enviar_pcb(new_pcb, socket_cpu, DISPATCH_PROCESO); //a partir de aca es el mismo codigo de Dani
		log_info(logger, "Solicitud DISPATCH_PROCESO enviada a CPU");
		pcb* pcb_respuesta;
		pcb_respuesta = esperar_pcb(socket_cpu, DISPATCH_PROCESO);
		loggear_pcb(pcb_respuesta);
		log_info(logger, "Respuesta DISPATCH_PROCESO recibida");

		list_add(procesoExecute,new_pcb); //paso a cola execute el primer proceso de la cola ready
		list_remove(procesoReady,0); // elimino de ready el proceso enviado a execute
	}
}
/*
void dispatch_proceso(pcb* new_pcb){
	enviar_pcb(new_pcb, socket_cpu, DISPATCH_PROCESO);
	log_info(logger, "Solicitud DISPATCH_PROCESO enviada a CPU");
	
	pcb* pcb_respuesta;
	pcb_respuesta = esperar_pcb(socket_cpu, DISPATCH_PROCESO);
	loggear_pcb(pcb_respuesta);

	log_info(logger, "Respuesta DISPATCH_PROCESO recibida");
}*/

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

static void planificadorFIFO(){
	log_info(logger,"Planificador FIFO iniciado.");
	int cantExecute;
	int cantReady;
	int cantBlock;
	int cantNew;
	while(1){
		//hago conteo de los procesos en cada cola de estado; uso semaforo por buena practica visto en otros tps
		sem_wait(&bloque);//modificar, el uso de semaforos esta mal implementado
			cantExecute = list_size(procesoExecute);
			cantReady = list_size(procesoReady);
			cantBlock = list_size(procesoBlock);
			cantNew = list_size(procesoNew);
		sem_post(&bloque); //modificar, el uso de semaforos esta mal implementado
		
		if (cantNew>0){ //planificador pasa de NEW a READY si el grado de multiprogramacion lo permite
			if(cantReady<config->gradoMultiprogramacion){
				int contador=0;
				while (cantReady<config->gradoMultiprogramacion && cantNew>0){
					sem_wait(&bloque);//modificar, el uso de semaforos esta mal implementado
					pcb* PCBtemporal = list_get(procesoNew,0);
					PCBtemporal->estado = READY; //cambios de estado se informan a memoria?
					list_remove(procesoNew,0);
					list_add(procesoReady,PCBtemporal);
					cantNew--;
					cantReady++;
					log_info(logger,"Se paso el proceso <%d> de NEW a READY", PCBtemporal->pid);
					sem_post(&bloque);//modificar, el uso de semaforos esta mal implementado
				}
			}
		}
		if(cantBlock>0){ //aca deberia usar semaforos para indicar que se libera un bloqueo de IO?
			if(cantReady<config->gradoMultiprogramacion){
				int contador=0;
				while (cantReady<config->gradoMultiprogramacion && cantBlock>0){
					sem_wait(&bloque);//modificar, el uso de semaforos esta mal implementado
					pcb* PCBtemporal = list_get(procesoBlock,0);
					PCBtemporal->estado = READY; //cambios de estado se informan a memoria?
					list_remove(procesoBlock,0);
					list_add(procesoReady,PCBtemporal);
					cantBlock--;
					cantReady++;
					log_info(logger,"Se paso el proceso <%d> de BLOCK a READY", PCBtemporal->pid);
					sem_post(&bloque);//modificar, el uso de semaforos esta mal implementado
				}
			}
		}
		if (cantReady>0) {
			while (cantReady>0 && cantExecute<config->gradoMultiprogramacion){
			dispatch_proceso();
			cantReady--;
			cantExecute++;
			}
		}
	}
}

static void planificadorRR() {
    log_info(logger, "Planificador Round Robin iniciado.");
    int cantExecute;
    int cantReady;
    int cantBlock;
    int cantNew;

    while (1) {
        // Conteo de procesos en cada cola de estado
        sem_wait(&bloque);
        cantExecute = list_size(procesoExecute);
        cantReady = list_size(procesoReady);
        cantBlock = list_size(procesoBlock);
        cantNew = list_size(procesoNew);
        sem_post(&bloque);

        // Movimiento de NEW a READY si el grado de multiprogramación lo permite
        if (cantNew > 0 && cantReady < config->gradoMultiprogramacion) {
            while (cantReady < config->gradoMultiprogramacion && cantNew > 0) {
                sem_wait(&bloque);
                pcb *PCBtemporal = list_get(procesoNew, 0);
                PCBtemporal->estado = READY;
                list_remove(procesoNew, 0);
                list_add(procesoReady, PCBtemporal);
                cantNew--;
                cantReady++;
                log_info(logger, "Se pasó el proceso <%d> de NEW a READY", PCBtemporal->pid);
                sem_post(&bloque);
            }
        }

        // Despachar procesos a EXECUTE hasta el grado de multiprogramación
        if (cantReady > 0 && cantExecute < config->gradoMultiprogramacion) {
            while (cantReady > 0 && cantExecute < config->gradoMultiprogramacion) {
                sem_wait(&bloque);
                pcb *PCBtemporal = list_get(procesoReady, 0);
                PCBtemporal->estado = EXECUTE;
                list_remove(procesoReady, 0);
                list_add(procesoExecute, PCBtemporal);
                dispatch_proceso(PCBtemporal);
                cantReady--;
                cantExecute++;
                sem_post(&bloque);
            }
        }

        // Control del quantum
        if (cantExecute > 0) {
            long inicioReloj = get_current_time_millis();
            long quantumTranscurrido = 0;

            // Bucle while que controla el tiempo de quatum
            while (quantumTranscurrido < config->quantum) {
                quantumTranscurrido = get_current_time_millis() - inicioReloj;
            }
            sem_wait(&bloque);
            for (int i = 0; i < list_size(procesoExecute); i++) {
                pcb *PCBtemporal = list_get(procesoExecute, i);
                PCBtemporal->quantum -= config->quantum;
                if (PCBtemporal->quantum <= 0) {
                    PCBtemporal->estado = EXIT;
                    list_remove(procesoExecute, i);
                } else {
                    PCBtemporal->estado = READY;
                    list_remove(procesoExecute, i);
                    list_add(procesoReady, PCBtemporal);
                    log_info(logger, "Se pasó el proceso <%d> de EXECUTE a READY", PCBtemporal->pid);
                }
            }
            sem_post(&bloque);
            enviar_interrupt(); // Simula enviar una interrupción a la CPU
        }
    	// Movimiento de BLOCK a READY si el grado de multiprogramación lo permite
        if (cantBlock > 0 && cantReady < config->gradoMultiprogramacion) {
            while (cantReady < config->gradoMultiprogramacion && cantBlock > 0) {
                sem_wait(&bloque);
                pcb *PCBtemporal = list_get(procesoBlock, 0);
                PCBtemporal->estado = READY;
                list_remove(procesoBlock, 0);
                list_add(procesoReady, PCBtemporal);
                cantBlock--;
                cantReady++;
                log_info(logger, "Se pasó el proceso <%d> de BLOCK a READY", PCBtemporal->pid);
                sem_post(&bloque);
            }
        }
	}
}

static void planificadorVRR(){
	//TODO
}
static void planificador(){
	if(strcmp(config->algoritmo_planificacion,"FIFO")==0){
		planificadorFIFO();
	}else if (strcmp(config->algoritmo_planificacion,"RR")==0){
		planificadorRR();
	}else if (strcmp(config->algoritmo_planificacion,"VRR")==0){
		planificadorVRR();
	}
}
/*
El módulo Kernel, en el contexto de nuestro trabajo práctico, será el encargado de gestionar la ejecución de los 
diferentes procesos que se generen por medio de su consola interactiva.

*/