#include "../include/gestor_io.h"


void *escuchar_io(void *socket){
	int cliente_fd = *((int*) socket);

	t_list* lista;
	bool on = 1;
	while (on) {
        log_info(logger, "Esperando operación desde Kernel...");
		int cod_op = recibir_operacion(cliente_fd);

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

bool validar_y_enviar_instruccion_a_io(char** instruc_io_tokenizadas){
    conexion_io* conexion_io;

    if(!existe_io_conectada(instruc_io_tokenizadas[1])){
        log_error(logger, "La IO no existe: [%s]", instruc_io_tokenizadas[1]);
        return false;
    }

    conexion_io = obtener_conexion_io_por_nombre(instruc_io_tokenizadas[1]);

    if(!io_acepta_operacion(*conexion_io, instruc_io_tokenizadas[0])){
        log_error(logger, "La IO no acepta la operacion: [%s]", instruc_io_tokenizadas[0]);
        return false;
    }

    if(!io_disponible(*conexion_io)){
        // Pediente logica de esperar si IO no está disponible
        log_error(logger, "La IO no está disponible");
        return false;
    }

    enviar_instruccion_io(instruc_io_tokenizadas, conexion_io->socket);

    return true;
}


conexion_io* obtener_conexion_io_por_nombre(char* nombre_io){
    return (conexion_io*) list_get(lista_conexiones_io, 0);
}

// Validaciones

bool existe_io_conectada(char* nombre_io){
    log_info(logger, "validando si existe io...");
    log_info(logger, "Nombre_io: [%s]", nombre_io);

    //t_list_iterator* iterator = list_iterator_create(lista_conexiones_io);

    //for(; list_iterator_has_next(iterator); iterator->next){
      //  conexion_io* conex = iterator->actual;
        
        //log_info(logger, "conex->nombre_interfaz: [%s]", conex->nombre_interfaz);
        //if(strcmp(nombre_io, conex->nombre_interfaz) == 0)
          //  return true;
    //}

    return true;
}

bool io_acepta_operacion(conexion_io conexion_io,char* operacion_io){
    return true;
}

bool io_disponible(conexion_io conexion_io){
    return true;
}
