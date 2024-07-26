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
	inicializar_recursos();

	log_info(logger, "Creando hilo para recibir conexiones io...");
	iniciar_hilo(esperar_y_escuchar_conexiones_io);

	log_info(logger, "Creando hilo para el planificador de largo plazo...");
	iniciar_hilo(iniciar_planificacion_largo);
	
	crear_hilo_planificador_corto();
	
	iniciar_consola();

	terminar_programa();

	return EXIT_SUCCESS;
}

void* esperar_y_escuchar_conexiones_io(){
    lista_conexiones_io = list_create();

    while(1){
        conexion_io* conexion_io = recibir_conexion_io(socket_server_kernel);

		pthread_mutex_lock(&mutex_conexiones_io);
			list_add(lista_conexiones_io, conexion_io);
		pthread_mutex_unlock(&mutex_conexiones_io);
		
		iniciar_hilo_con_args(escuchar_io, conexion_io);
    }
}

void iniciar_semaforos() {
	sem_init(&sem_nuevo_proceso, 0, 0);
	sem_init(&sem_proceso_en_ready, 0, 0);
	sem_init(&sem_cpu_libre, 0, 1);
	sem_init(&sem_grado_multiprog, 0, config->grado_multiprogramacion);
	sem_init(&sem_planificacion_activa, 0, 0);
}

void terminar_programa()
{
    liberar_conexion(socket_cpu_dispatch);
    liberar_conexion(socket_memoria);

    log_info(logger, "Memoria liberada correctamente");
    log_destroy(logger);
}

void crear_hilo_planificador_corto(){
	log_info(logger, "algortimo: %s", config->algoritmo_planificacion);
	if(string_equals_ignore_case(config->algoritmo_planificacion, "FIFO")){
		log_info(logger, "Creando hilo para el planificador de corto plazo FIFO...");
		iniciar_hilo(iniciar_planificacion_corto);
	}else if(string_equals_ignore_case(config->algoritmo_planificacion, "RR")){
		log_info(logger, "Creando hilo para el planificador de corto plazo ROUND ROBIN...");
		iniciar_hilo(iniciar_planificacion_corto_RR);
	}else if(string_equals_ignore_case(config->algoritmo_planificacion, "VRR")){
		log_info(logger, "Creando hilo para el planificador de corto plazo VIRTUAL ROUND ROBIN...");
		iniciar_hilo(iniciar_planificacion_corto_VRR);
	}else{
		log_error(logger, "Algoritmo de planificación desconocido.");
		terminar_programa();
	}
}

//void cancelar_hilo_planificador(){
//	if (string_equals_ignore_case(config->algoritmo_planificacion,"FIFO")){
//		pthread_cancel(hilo_planificador_corto);
//		log_info(logger,"HILO PLANIFICADOR FIFO CANCELADO");
//	}else if(string_equals_ignore_case(config->algoritmo_planificacion,"RR")){
//		pthread_cancel(hilo_planificador_corto_RR);
//		log_info(logger,"HILO PLANIFICADOR RR CANCELADO");
//	}else if(string_equals_ignore_case(config->algoritmo_planificacion,"VRR")){
//		pthread_cancel(hilo_planificador_corto_VRR);
//		log_info(logger,"HILO PLANIFICADOR VRR CANCELADO");
//	}
//}

