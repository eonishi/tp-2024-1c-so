#include "../include/conexion.h"

int socket_memoria, socket_kernel;
int INTERRUPTION_FLAG = 0;


static void terminar_programa()
{
	log_info(logger, "Memoria liberada correctamente");
	log_destroy(logger);
}

int crear_conexion_memoria()
{
  log_info(logger, "Creando conexión con Memoria...");
	int conexion = crear_conexion(config.ip_memoria, config.puerto_memoria);
	log_info(logger, "Conexión creada. Socket: %i", conexion);
	socket_memoria = conexion;

	enviar_handshake(socket_memoria);
	if(recibir_operacion(socket_memoria) != HANDSHAKE){
		log_error(logger, "Error en el handshake con Memoria");
		abort();
	}
	TAM_PAGINA = recibir_cantidad(socket_memoria); // Asignacion a variable que usa la MMU
	log_info(logger, "TAM_PAGINA: %d", TAM_PAGINA);

  return conexion;
}

void esperar_handshake_kernel(int server) {
    log_info(logger,"Esperando conexión del modulo Kernel ... ");
    socket_kernel = esperar_cliente(server);
	log_info(logger,"Esperando handshake del modulo Kernel ... ");
    int resultado = esperar_handshake(socket_kernel);
    if(resultado == -1) {
        log_error(logger,"No se pudo conectar con el modulo KERNEL");
        exit(EXIT_FAILURE);
    }

	log_info(logger,"Respondiendo handshake del modulo Kernel ... ");
    enviar_handshake(socket_kernel);
}

int esperar_handshake_server(int server) {
    log_info(logger,"Esperando conexión del modulo Kernel ... ");
    int socket = esperar_cliente(server);
	log_info(logger,"Esperando handshake del modulo Kernel ... ");
    int resultado = esperar_handshake(socket);
    if(resultado == -1) {
        log_error(logger,"No se pudo conectar con el modulo KERNEL");
        exit(EXIT_FAILURE);
    }

	log_info(logger,"Respondiendo handshake del modulo Kernel ... ");
    enviar_handshake(socket);
	return socket;
}


void server_dispatch()
{
	int dispatch_fd = iniciar_servidor("CPU_DISPATCH", config.ip_cpu, config.puerto_dispatch);
  
  	socket_kernel = esperar_handshake_server(dispatch_fd);

	while (1)
	{
		log_info(logger, "Estoy por recibir operacion");//log_trace
		int cod_op = recibir_operacion(socket_kernel);//socket_kernel)
		log_info(logger, "Codigo recibido: %d", cod_op);

		switch (cod_op)
		{
		case DISPATCH_PROCESO:
			log_info(logger, "==============================================");
            log_info(logger, "DISPATCH_PROCESO recibido. CODIGO: %d", cod_op);

			pcb_actual = recibir_pcb(socket_kernel);//QUEDA BLOQUEADO ACÁ
			if (!pcb_actual) {//agregado por mollon
                log_error(logger, "Error al recibir el PCB");
                break;
            }
			log_info(logger,"recibi pcb");//agregado por mollon
			tengo_pcb = 1;

			cicloDeCPU();			

			log_info(logger, "Fin DISPATCH_PROCESO");
			log_info(logger, "==============================================");

			loggear_pcb(pcb_actual);

			free(pcb_actual);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			terminar_programa();
			exit(EXIT_FAILURE);
		default:
			log_warning(logger, "Operacion desconocida. No quieras meter la pata");
			break;
		}
		log_info(logger, "==============================================");
	}
}

void server_interrupt()
{
	int interrupt_fd = iniciar_servidor("CPU_INTERRUPT", config.ip_cpu, config.puerto_interrupt);
	int cliente_fd = esperar_handshake_server(interrupt_fd);

	//int cliente_fd = esperar_cliente(interrupt_fd);
	log_info(logger, "pase de esperar cliente!! interrupt_fd es %d",interrupt_fd);

	while (cliente_fd != -1)
	{
		unsigned PID_interrumpido = recibir_cantidad(cliente_fd);
		log_info(logger, "Recibi interrupcion a PID: %d", PID_interrumpido);

		if(PID_interrumpido == pcb_actual->pid){
			log_info(logger, "INTERRUPCION recibida. PID: %d", PID_interrumpido);
			INTERRUPTION_FLAG = 1;
		}
		else{
			log_info(logger, "INTERRUPCION recibida. PID: %d. No corresponde al PID actual", PID_interrumpido);
		}
	}
	log_error(logger, "el cliente se desconecto. Terminando servidor");
	terminar_programa();
	exit(EXIT_FAILURE);
}