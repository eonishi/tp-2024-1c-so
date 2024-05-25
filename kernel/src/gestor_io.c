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

bool existe_io_conectada(char* nombre_io){
    return true;
}

conexion_io* obtener_conexion_io_por_nombre(char* nombre_io){
    return (conexion_io*) list_get(lista_conexiones_io, 0);
}

bool io_acepta_operacion(conexion_io conexion_io,char* operacion_io){
    return true;
}

bool io_disponible(conexion_io conexion_io){
    return true;
}


bool validar_y_enviar_instruccion_a_io(char** instruc_io_tokenizadas){
    conexion_io* conexion_io;

    if(!existe_io_conectada(instruc_io_tokenizadas[1])){
        log_error(logger, "La IO no existe: [%s]", instruc_io_tokenizadas[0]);

        return false;
    }

    log_info(logger, "La io existe");
    conexion_io = obtener_conexion_io_por_nombre(instruc_io_tokenizadas[1]);

    if(!io_acepta_operacion(*conexion_io, instruc_io_tokenizadas[0])){
        log_error(logger, "La IO no existe: [%s]", instruc_io_tokenizadas[0]);
    }

    log_info(logger, "Y la operación es válida.");

    if(!io_disponible(*conexion_io)){
        log_error(logger, "La IO no está disponible");
    }

    enviar_instruccion_io(instruc_io_tokenizadas, conexion_io->socket);

    return true;
}