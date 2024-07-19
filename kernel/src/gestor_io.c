#include "../include/gestor_io.h"


void *escuchar_io(void *socket){
	int cliente_fd = *((int*) socket);

	bool on = 1;
	while (on) {
        log_info(logger, "Esperando operación desde Kernel...");
		int cod_op = recibir_operacion(cliente_fd);

		switch (cod_op) {
		case FIN_EJECUCION_IO:
			log_info(logger, "Recibi FIN_EJECUCION_IO. CODIGO: %d", cod_op);

			pcb* pcb_blocked = pop_cola_blocked();
			pcb_blocked->estado = READY;
            //aca se envia a ready luego de desbloquearse por I/O
            if(strcmp(config->algoritmo_planificacion, "VRR") == 0){
                temporal_stop(q_transcurrido); // detengo el contador iniciado en dispatch (if algortimo VRR)
                int contador_quantum_cast = temporal_gettime(q_transcurrido); //casteo el contador a milisegundos
                push_cola_ready_priority(pcb_blocked, contador_quantum_cast); 
			    sem_post(&sem_proceso_en_ready);//chequear si hace falta otro semaforo por VRR, no deberia.
            }else {
			    push_cola_ready(pcb_blocked);
			    sem_post(&sem_proceso_en_ready);
            }
			break;
		case DISPATCH_PROCESO:
            log_info(logger, "Recibi DISPATCH_PROCESO. CODIGO: %d", cod_op);
			pcb* pcb = recibir_pcb(cliente_fd);
			loggear_pcb(pcb);
			log_info(logger, "Respuesta DISPATCH_PROCESO recibida");
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

bool validar_y_enviar_instruccion_a_io(char** instruc_io_tokenizadas, t_list* peticiones_memoria){
    conexion_io* conexion_io;
    char* instruccion = instruc_io_tokenizadas[0];
    char* nombre_io = instruc_io_tokenizadas[1];

    if(!existe_io_conectada(nombre_io)){
        log_error(logger, "La IO no existe: [%s]", nombre_io);
        return false;
    }

    conexion_io = obtener_conexion_io_por_nombre(nombre_io);

    // SOLO admite operaciones de tipo IO_GEN_SLEEP
    if(!io_acepta_operacion(*conexion_io, instruccion)){
        log_error(logger, "La IO no acepta la operacion: [%s]", instruccion);
        log_operaciones(conexion_io->operaciones, conexion_io->tipo);
        return false;
    }
    log_info(logger, "La IO acepta la operacion: [%s]", instruccion);

    // TODO: Deberia gestionarse con semaforos
    if(!io_disponible(*conexion_io)){
        // Pediente logica de esperar si IO no está disponible
        log_error(logger, "La IO no está disponible");
        return false;
    }

    char* file_name;

    switch (get_operacion(instruc_io_tokenizadas)){
        case IO_GEN_SLEEP:
        case IO_STDIN_READ:
        case IO_STDOUT_WRITE:
            // [] Enviar peticiones de memoria
            log_info(logger, "Enviando instruccion a IO...");
            enviar_instruccion_io(instruc_io_tokenizadas, peticiones_memoria, conexion_io->socket);
            break;
        case IO_FS_CREATE:
            file_name = instruc_io_tokenizadas[2];
            enviar_mensaje(CREAR_ARCHIVO_FS, file_name, conexion_io->socket);
            break;
        case IO_FS_DELETE:
            file_name = instruc_io_tokenizadas[2];
            enviar_mensaje(ELIMINAR_ARCHIVO_FS, file_name, conexion_io->socket);
            break;
        case IO_FS_TRUNCATE:
            file_name = instruc_io_tokenizadas[2];
            char* size = instruc_io_tokenizadas[3];
            
            solicitud_truncar_archivo solicitud = {file_name, atoi(size)};

            enviar_solicitud_truncar_archivo_fs(solicitud, conexion_io->socket);

            break;
        case IO_FS_WRITE:
            enviar_instruccion_io(instruc_io_tokenizadas, peticiones_memoria, conexion_io->socket);
            
            break;
        case IO_FS_READ:
            log_error(logger, "Operacion no implementada");
            break;
        default:
            log_error(logger, "Operacion no reconocida");
            break;
    }
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
