#include "../include/conexion.h"

t_list *lista_conexiones_io;
pthread_mutex_t mutex_conexiones_io = PTHREAD_MUTEX_INITIALIZER;

bool generar_conexiones(){
    log_info(logger, "Creando conexión con CPU dispatch...");

    socket_cpu_dispatch = generar_conexion(config->ip_cpu, config->puerto_cpu_dispatch);

    if(socket_cpu_dispatch == -1)
        return false;

    log_info(logger, "Creando conexión con MEMORIA...");
    socket_memoria = generar_conexion(config->ip_memoria, config->puerto_memoria);

    if(socket_memoria == -1)
        return false;
    
    socket_server_kernel = iniciar_servidor("KERNEL", config->ip_kernel, config->puerto_kernel);
	// socket_io = recibir_conexion_io(socket_server_kernel);

    //if(socket_io == -1)
     //   return false;

    log_info(logger, "Creando conexión con CPU interrupt...");

    socket_cpu_interrupt = generar_conexion(config->ip_cpu, config->puerto_cpu_interrupt);

    if(socket_cpu_interrupt == -1)
        return false;

    return true;
}

conexion_io* recibir_conexion_io(int server) {
    log_info(logger,"Esperando conexión del modulo IO ... ");
    int socket = esperar_cliente(server);

	log_info(logger,"Esperando handshake del modulo IO ... ");
    int cod = recibir_operacion(socket);
    if(cod != HANDSHAKE){
        log_error(logger, "Respuesta erronea de handshake");
        close(socket);
        exit(EXIT_FAILURE); // Nose si mata al proceso o solo al hilo
    }
    solicitud_conexion_kernel* solicitud = recibir_solicitud_conexion_kernel(socket);
    log_info(logger,"Recibido handshake del modulo IO. Interfaz: [%s] Tipo: [%d] Operacion: [%d]", solicitud->nombre_interfaz, solicitud->tipo, solicitud->operaciones[0]);
    log_info(logger, "Respondiendo handshake del modulo IO... ");
    enviar_status(SUCCESS, socket);
    
    conexion_io* conexion_io = crear_conexion_io(socket, solicitud->nombre_interfaz, solicitud->tipo, solicitud->operaciones);

    liberar_solicitud_conexion_kernel(solicitud);
    return conexion_io;
}

int generar_conexion(char* ip, char* puerto){
	int conexion = crear_conexion(ip, puerto);
    if(conexion == -1){
        log_error(logger, "No se pudo conectar al servidor. IP: [%s] Puerto: [%s] Socket:[%d]", ip, puerto, conexion);    
        return -1;
    }

	log_info(logger, "Conexión creada. Socket: %i", conexion);

	enviar_handshake(conexion);
	if(esperar_handshake(conexion) == -1){
        log_error(logger, "Respuesta erronea de handshake");
    };

    log_info(logger, "Handshake realizado enviado y recibido correctamente");

    return conexion;
}

conexion_io* crear_conexion_io(int socket, char* nombre_interfaz, io_tipo tipo, int* operaciones){
    conexion_io* conexion = (conexion_io*)malloc(sizeof(conexion_io));
    conexion->socket = socket;
    conexion->nombre_interfaz = nombre_interfaz;
    conexion->tipo = tipo;
    conexion->operaciones = operaciones;
    conexion->cola_espera = queue_create();
    sem_init(&conexion->sem_cliente, 0, 0);
    pthread_mutex_init(&conexion->mutex, NULL);

    return conexion;
}

void liberar_conexion_io(conexion_io* conexion_io){
    free(conexion_io->nombre_interfaz);
    free(conexion_io->operaciones);
    free(conexion_io);
}