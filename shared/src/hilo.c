#include "../include/hilo.h"

void iniciar_hilo(void* function_handler){
    pthread_t thread;
	int err = pthread_create(&thread, NULL, function_handler, NULL);

    if (err != 0){
    	log_info(logger, "Hubo un problema al crear el hilo. Error: [%s]", strerror(err));
    }
	else{
    	log_info(logger, "El hilo se inició su correctamente");
		pthread_detach(thread);
	}
}

void iniciar_hilo_con_args(void *(*function_handler)(void *), void* args){
    pthread_t thread;
	int err = pthread_create(&thread, NULL, function_handler, args);

    if (err != 0){
    	log_info(logger, "Hubo un problema al crear el hilo. Error: [%s]", strerror(err));
    }
	else{
    	log_info(logger, "El hilo se inició su correctamente");
		pthread_detach(thread);
	}
}