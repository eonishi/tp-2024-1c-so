#include "include/kernel.h"


int main(){
    logger = iniciar_logger("kernel.log", "KERNEL");
	aux_log = iniciar_logger("aux_kernel.log", "KERNEL");
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
	
	crear_hilo_planificador_corto();
	
	iniciar_consola();

	terminar_programa();

	return EXIT_SUCCESS;
}

void* esperar_y_escuchar_conexiones_io(){
    lista_conexiones_io = list_create();
	pthread_t io_threads[10]; // TODO Mejorar
	int io_thread_index = 0;

    while(1){
        conexion_io* conexion_io = recibir_conexion_io(socket_server_kernel);

		list_add(lista_conexiones_io, conexion_io);
		iniciar_hilo_con_args(escuchar_io, io_threads[io_thread_index++], &conexion_io->socket);
    }
}


void iniciar_semaforos() {
	sem_init(&sem_nuevo_proceso, 0, 0);
	sem_init(&sem_proceso_en_ready, 0, 0);
	sem_init(&sem_cpu_libre, 0, 1);
	sem_init(&sem_grado_multiprog, 0, config->grado_multiprogramacion);
}

void terminar_programa()
{
    liberar_conexion(socket_cpu_dispatch);
    liberar_conexion(socket_memoria);

    log_info(logger, "Memoria liberada correctamente");
    log_destroy(logger);
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

/*void cambiar_algoritmo_planificadorCorto(algoritmo_planificador algoritmoNuevo){
	//todo
}*/