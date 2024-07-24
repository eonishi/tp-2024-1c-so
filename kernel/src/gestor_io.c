#include "../include/gestor_io.h"

static void enviar_instruccion_io_segun_op(pcb* pcb_io, conexion_io* conexion_io){
    operacion operacion = get_operacion(pcb_io->solicitud->instruc_io_tokenizadas);
    log_info(logger, "Enviando operacion [%d] a IO [%s]", operacion, conexion_io->nombre_interfaz);

    switch (operacion){
        case IO_GEN_SLEEP:
            enviar_cantidad(atoi(pcb_io->solicitud->instruc_io_tokenizadas[2]), EJECUTAR_INSTRUCCION_IO, conexion_io->socket);
            break;
        case IO_STDIN_READ:
        case IO_STDOUT_WRITE:
            enviar_instruccion_io(
                pcb_io->solicitud->instruc_io_tokenizadas,
                pcb_io->solicitud->peticiones_memoria,
                conexion_io->socket
            );
            break;
        case IO_FS_CREATE:
        case IO_FS_DELETE:
            //char* file_name = instruc_io_tokenizadas[2];
            //enviar_mensaje(EJECUTAR_INSTRUCCION_IO, file_name, conexion_io->socket);
            break;
        case IO_FS_TRUNCATE:
        case IO_FS_READ:
        case IO_FS_WRITE:
            log_error(logger, "Operacion no implementada");
        default:
            log_error(logger, "Operacion no reconocida");
            break;
    }
}


void *escuchar_io(void *args){
	conexion_io* conexion = (conexion_io*) args;
    log_info(logger, "Escuchando IO [%s]...", conexion->nombre_interfaz);

	bool on = 1;
	while (on) {

        // Espero a tener algo en la cola
        sem_wait(&conexion->sem_cliente);

        pcb* pcb_blocked = queue_peek(conexion->cola_espera);
        enviar_instruccion_io_segun_op(pcb_blocked, conexion);

        log_info(logger, "Esperando operación desde Kernel...");
		int cod_op = recibir_operacion(conexion->socket);

		switch (cod_op) {
		case FIN_EJECUCION_IO:
			log_info(logger, "Recibi FIN_EJECUCION_IO. CODIGO: %d", cod_op);

            // Limpio el buffer de solicitud_a_io del PCB
            solicitud_bloqueo_por_io* solicitud_realizada = pcb_blocked->solicitud;
            pcb_blocked->solicitud = NULL;
            liberar_solicitud_io(solicitud_realizada);

            pcb* pcb_to_ready = queue_pop(conexion->cola_espera);
            
            //aca se envia a ready luego de desbloquearse por I/O
            if(strcmp(config->algoritmo_planificacion, "VRR") == 0){
                temporal_stop(q_transcurrido); // detengo el contador iniciado en dispatch (if algortimo VRR)
                int contador_quantum_cast = temporal_gettime(q_transcurrido); //casteo el contador a milisegundos
                push_cola_ready_priority(pcb_blocked, contador_quantum_cast); 
			    sem_post(&sem_proceso_en_ready);//chequear si hace falta otro semaforo por VRR, no deberia.
            }else {
			    push_cola_ready(pcb_blocked);
            }
			  break;
        
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
            cerrar_conexion_io(conexion->nombre_interfaz);
			on = 0;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
}

bool validar_instruccion_a_io(char** instruc_io_tokenizadas, pcb* pcb){
    log_warning(logger, "Por crear una conexion IO");
    conexion_io* conexion_io;
    char* instruccion = instruc_io_tokenizadas[0];
    log_warning(logger, "Pude asignar a instruccion");
    char* nombre_io = instruc_io_tokenizadas[1];
    log_warning(logger, "Pude asignar el nombre de la interfaz");

    log_warning(logger, "Validando instruccion a IO...");
    if(!existe_io_conectada(nombre_io)){
        log_error(logger, "La IO no existe: [%s]", nombre_io);
        return false;
    }

    conexion_io = obtener_conexion_io_por_nombre(nombre_io);

    if(!io_acepta_operacion(*conexion_io, instruccion)){
        log_error(logger, "La IO no acepta la operacion: [%s]", instruccion);
        log_operaciones(conexion_io->operaciones, conexion_io->tipo);
        return false;
    }

    log_info(logger, "La IO acepta la operacion: [%s]", instruccion);
    return true;
}

static char *interfaz_buscada;
static bool interfaz_es_la_buscada(void *conexion_buscada){
    conexion_io* conexion = (conexion_io*) conexion_buscada;
    return string_equals_ignore_case(conexion->nombre_interfaz, interfaz_buscada);
}

conexion_io* obtener_conexion_io_por_nombre(char* nombre_io){
    pthread_mutex_lock(&mutex_interfaz_buscada);
        interfaz_buscada = nombre_io;
        conexion_io* result = list_find(lista_conexiones_io, interfaz_es_la_buscada);
    pthread_mutex_unlock(&mutex_interfaz_buscada);
    return result;
}

// Validaciones
bool existe_io_conectada(char* nombre_io){
    log_info(logger, "Validando si existe IO [%s]", nombre_io);

    pthread_mutex_lock(&mutex_interfaz_buscada);
        interfaz_buscada = nombre_io;
        bool result = list_any_satisfy(lista_conexiones_io, interfaz_es_la_buscada);
    pthread_mutex_unlock(&mutex_interfaz_buscada);
    return result;
}

bool io_acepta_operacion(conexion_io conexion_io, char* operacion_io){
    return io_tiene_operacion(conexion_io.operaciones, operacion_io, conexion_io.tipo);
}

void cerrar_conexion_io(char* nombre_io){
    pthread_mutex_lock(&mutex_interfaz_buscada);
        interfaz_buscada = nombre_io;
        conexion_io* conexion_eliminada = list_remove_by_condition(lista_conexiones_io, interfaz_es_la_buscada);
    pthread_mutex_unlock(&mutex_interfaz_buscada);

    log_info(logger, "Conexion eliminada: [%s]", conexion_eliminada->nombre_interfaz);
    liberar_conexion_io(conexion_eliminada);
}

void enviar_proceso_a_esperar_io(pcb* pcb_a_espera){
    char* nombre_io = pcb_a_espera->solicitud->instruc_io_tokenizadas[1];
    log_warning(logger, "Nombre_io: [%s]", nombre_io);    

    conexion_io* conexion_io = obtener_conexion_io_por_nombre(nombre_io);
    push_cola_blocked(pcb_a_espera, conexion_io->cola_espera, &conexion_io->sem_cliente);
}