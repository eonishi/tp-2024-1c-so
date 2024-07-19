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
		iniciar_hilo_con_args(escuchar_io, io_threads[io_thread_index++], conexion_io);
    }
}

pthread_mutex_t mutex_interfaz_buscada;

void iniciar_semaforos() {
	sem_init(&sem_nuevo_proceso, 0, 0);
	sem_init(&sem_proceso_en_ready, 0, 0);
	sem_init(&sem_cpu_libre, 0, 1);
	sem_init(&sem_grado_multiprog, 0, config->grado_multiprogramacion);

	pthread_mutex_init(&mutex_interfaz_buscada, NULL);
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

void crear_hilo_planificador_corto(){
	log_info(logger, "algortimo: %s", config->algoritmo_planificacion);
	if(string_equals_ignore_case(config->algoritmo_planificacion, "FIFO")){
		log_info(logger, "Creando hilo para el planificador de corto plazo FIFO...");
		iniciar_hilo(iniciar_planificacion_corto, hilo_planificador_corto);
	}else if(string_equals_ignore_case(config->algoritmo_planificacion, "RR")){
		log_info(logger, "Creando hilo para el planificador de corto plazo ROUND ROBIN...");
		iniciar_hilo(iniciar_planificacion_corto_RR, hilo_planificador_corto_RR);
	}else if(string_equals_ignore_case(config->algoritmo_planificacion, "VRR")){
		log_info(logger, "Creando hilo para el planificador de corto plazo VIRTUAL ROUND ROBIN...");
		iniciar_hilo(iniciar_planificacion_corto_VRR, hilo_planificador_corto_VRR);
	}else{
		log_error(logger, "Algoritmo de planificación desconocido.");
		terminar_programa();
	}
}

void cancelar_hilo_planificador(){
	if (string_equals_ignore_case(config->algoritmo_planificacion,"FIFO")){
		pthread_cancel(hilo_planificador_corto);
		log_info(logger,"HILO PLANIFICADOR FIFO CANCELADO");
	}else if(string_equals_ignore_case(config->algoritmo_planificacion,"RR")){
		pthread_cancel(hilo_planificador_corto_RR);
		log_info(logger,"HILO PLANIFICADOR RR CANCELADO");
	}else if(string_equals_ignore_case(config->algoritmo_planificacion,"VRR")){
		pthread_cancel(hilo_planificador_corto_VRR);
		log_info(logger,"HILO PLANIFICADOR VRR CANCELADO");
	}
}
