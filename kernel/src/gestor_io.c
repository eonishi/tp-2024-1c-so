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
            // Escuchar_IO deberia traer como parametro tambien el nombre de la interfaz
            // para poder eliminarla de la lista de conexiones cuando se desconecte.
            // eliminar_conexion_io(nombre_interfaz);
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

    // SOLO admite operaciones de tipo IO_GEN_SLEEP
    if(!io_acepta_operacion(*conexion_io, instruc_io_tokenizadas[0])){
        log_error(logger, "La IO no acepta la operacion: [%s]", instruc_io_tokenizadas[0]);
        return false;
    }

    // TODO: Deberia gestionarse con semaforos
    if(!io_disponible(*conexion_io)){
        // Pediente logica de esperar si IO no está disponible
        log_error(logger, "La IO no está disponible");
        return false;
    }

    enviar_instruccion_io(instruc_io_tokenizadas, conexion_io->socket);

    return true;
}

static char *interfaz_buscada;
static bool interfaz_es_la_buscada(conexion_io *conexion_io){
    return string_equals_ignore_case(conexion_io->nombre_interfaz, interfaz_buscada);
}

conexion_io* obtener_conexion_io_por_nombre(char* nombre_io){
    interfaz_buscada = nombre_io;
    conexion_io* result = list_find(lista_conexiones_io, (void*) interfaz_es_la_buscada);
    return result;
}

// Validaciones

bool existe_io_conectada(char* nombre_io){
    log_info(logger, "validando si existe io...");
    log_info(logger, "Nombre_io: [%s]", nombre_io);

    interfaz_buscada = nombre_io;
    return list_any_satisfy(lista_conexiones_io, (void*) interfaz_es_la_buscada);
}

bool io_acepta_operacion(conexion_io conexion_io, char* operacion_io){
    return io_tiene_operacion(conexion_io.operaciones, operacion_io, conexion_io.tipo);
}

bool io_disponible(conexion_io conexion_io){
    return true;
}

void eliminar_conexion_io(char* nombre_io){
    interfaz_buscada = nombre_io;
    list_remove_by_condition(lista_conexiones_io, (void*) interfaz_es_la_buscada);
}
