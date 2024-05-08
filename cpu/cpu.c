#include "include/cpu.h"

int main()
{
	logger = iniciar_logger("cpu.log", "CPU");
	log_info(logger, "Logger CPU Iniciado");

	inicializar_configuracion();  
  	crear_conexion_memoria();
	
	pthread_t server_threads[2];
	pthread_create(&server_threads[0], NULL, (void*)server_dispatch, NULL);
	pthread_create(&server_threads[1], NULL, (void*)server_interrupt, NULL);

	pthread_join(server_threads[0], NULL);
	pthread_join(server_threads[1], NULL);

	return EXIT_SUCCESS;
}

void terminar_programa()
{
	log_info(logger, "Memoria liberada correctamente");
	log_destroy(logger);
}

/*
El módulo CPU en nuestro contexto de TP lo que va a hacer es simular los pasos del ciclo de instrucción de una CPU real,
de una forma mucho más simplificada.
*/