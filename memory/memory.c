#include "include/memory.h"

int main(){
    logger = iniciar_logger("memory.log", "MEMORY", 1);
    logger_oblig = iniciar_logger("logs_obligatorios.log", "MEMORY", 1);

    log_info(logger, "Logger de MEMORY iniciado");
    
    inicializar_configuracion();

    int server_fd = iniciar_servidor("MEMORY", config.ip_memoria, config.puerto_memoria);

	// Inicializo la lista de procesos en memoria
	inicializar_procesos_en_memoria();
	inicializar_memoria();

	esperar_handshake_cpu(server_fd);
    esperar_handshake_kernel(server_fd);

    crear_hilo_solicitudes_cpu();    
    crear_hilo_solicitudes_kernel();
	while (1)
	{
		log_info(logger, "Esperando conexiones de IO...");
		if(servidor_escuchar_cliente(server_fd, gestionar_solicitudes_io)){
			log_info(logger, "Se conectó un cliente");
		}
		else{
			log_error(logger, "Se intento conectar un cliente pero no se pudo");
		}
		
	}
	
    terminar_programa();

    return EXIT_SUCCESS;
}

void crear_hilo_solicitudes_kernel(){
	int err = pthread_create(&(hilo_kernel), NULL, gestionar_solicitudes_kernel, NULL);
    if (err != 0){
    	log_info(logger, "Hubo un problema al crear el thread de la escucha del servidor [%s]", strerror(err));
    }
	else{
    	log_info(logger, "El thread de la escucha del servidor inició su ejecución");
		pthread_detach(hilo_kernel);
	}
}

void* gestionar_solicitudes_kernel(){
    log_info(logger, "Esperando recibir operacion del kernel...");
	while (socket_kernel != -1) {
		int cod_op = recibir_operacion(socket_kernel);
        log_info(logger, "==============================================");
        log_info(logger, "Codigo recibido desde el kernel: %d", cod_op);

		switch (cod_op) {
		case CREAR_PROCESO_EN_MEMORIA:
            log_info(logger, "CREAR_PROCESO_EN_MEMORIA recibido.");

            solicitud_crear_proceso solicitud = recibir_solicitud_crear_proceso(socket_kernel);
			esperar_retardo();
			bool carga_correcta = cargar_proceso_en_memoria(solicitud.filePath, solicitud.PID);

			if(carga_correcta){
				log_info(logger_oblig, "PID: <%u> - Páginas: <%d>", solicitud.PID, 0);
				enviar_status(SUCCESS, socket_kernel);
			}
			else {
				enviar_status(ERROR_DE_PROCESAMIENTO, socket_kernel);
			}

			break;
			
		case LIBERAR_PROCESO_EN_MEMORIA:
			// Recibir PID
			unsigned pid = recibir_cantidad(socket_kernel);
			log_info(logger, "Proceso PID:[%d] a liberar", pid);

			log_info(logger_oblig, "PID: <%d> - Páginas: <%ld>", PID_solicitado, cantidad_de_paginas());
			// Elimino el proceso de la lista de procesos en memoria
			liberar_proceso_en_memoria(pid);
			esperar_retardo();
			// Envio respuesta al kernel	
			enviar_status(SUCCESS, socket_kernel);
			break;

		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			close(socket_kernel);
			socket_kernel = -1;
			terminar_programa();
            break;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");

            terminar_programa();
			break;
		}
        log_info(logger, "==============================================");
	}
}

void iniciar_gestor_solicitudes_en_hilo(pthread_t hilo, void* funcion_gestor){
	int err = pthread_create(&(hilo), NULL, funcion_gestor, NULL);
    if (err != 0){
    	log_info(logger, "Hubo un problema al crear el thread de la escucha del servidor [%s]", strerror(err));
    }
	else{
    	log_info(logger, "El thread de la escucha del servidor inició su ejecución");
		pthread_detach(hilo);
	}
}

void crear_hilo_solicitudes_cpu(){
	int err = pthread_create(&(hilo_cpu), NULL, gestionar_solicitudes_cpu, NULL);
    if (err != 0){
    	log_info(logger, "Hubo un problema al crear el thread de la escucha del servidor [%s]", strerror(err));
    }
	else{
    	log_info(logger, "El thread de la escucha del servidor inició su ejecución");
        pthread_detach(hilo_cpu);		
	}
}

void* gestionar_solicitudes_cpu(){

    log_info(logger, "Esperando recibir operacion del CPU...");
	while (1) {    
		int cod_op = recibir_operacion(socket_cpu);
        log_info(logger, "==============================================");
        log_info(logger, "Codigo recibido desde el CPU: %d", cod_op);
		switch (cod_op) {
		case FETCH_INSTRUCCION:
            log_info(logger, "FETCH_INSTRUCCION recibido. ");

			recibir_fetch_de_cpu();
			esperar_retardo();
			enviar_instruccion_a_cpu();		
			break;

		case REDIMENSIONAR_MEMORIA_PROCESO:
			log_info(logger, "REDIMENSIONAR_MEMORIA_PROCESO recibido.");

			unsigned nuevo_tamanio_en_bytes = recibir_cantidad(socket_cpu);
			unsigned tamanio_actual_en_bytes = obtener_tamanio_actual_proceso();
			unsigned cantidad_de_paginas = calcular_cantidad_de_paginas_por_bytes(nuevo_tamanio_en_bytes);
		
			if(nuevo_tamanio_en_bytes > tamanio_actual_en_bytes)
				log_info(logger_oblig, "PID: <%d> - Tamaño Actual: <%d> - Tamaño a Ampliar: <%d>",
				PID_solicitado, tamanio_actual_en_bytes, nuevo_tamanio_en_bytes);
			else{
				log_info(logger_oblig, "PID: <%d> - Tamaño Actual: <%d> - Tamaño a Reducir: <%d>",
				PID_solicitado, tamanio_actual_en_bytes, nuevo_tamanio_en_bytes);
			}

			esperar_retardo();

			if(puedo_agregar_o_disminuir(cantidad_de_paginas)){
				redimensionar_memoria_proceso(cantidad_de_paginas, PID_solicitado);
				mostrar_tabla_paginas();

				enviar_status(SUCCESS, socket_cpu);
				log_info(logger, "Se redimensionó la memoria correctamente, envio: [%d]", SUCCESS);
				break;
			}
			else{
				enviar_status(OUT_OF_MEMORY, socket_cpu);
				log_info(logger, "No se pudo redimensionar la memoria, envio: [%d]", OUT_OF_MEMORY);
				break;
			}
			
		case ESCRIBIR_DATO_EN_MEMORIA:
			log_info(logger, "ESCRIBIR_DATO_EN_MEMORIA recibido.");

			t_peticion_memoria* solicitud = peticion_recibir(socket_cpu, ESCRIBIR_DATO_EN_MEMORIA);
			log_info(logger, "Recibido. Dirección_fisica: [%d], Tam_Dato: [%zu]", solicitud->direccion_fisica, solicitud->tam_dato);

			log_info(logger_oblig, "PID: <%d> - Accion: <%s> - Direccion fisica: <%d> - Tamaño <%ld>",
				PID_solicitado, "ESCRIBIR",solicitud->direccion_fisica, solicitud->tam_dato);

			set_memoria(solicitud->direccion_fisica, solicitud->dato, solicitud->tam_dato);

			esperar_retardo();
			enviar_status(SUCCESS, socket_cpu);
			log_info(logger, "Se escribió dato en memoria correctamente, envio: [%d]", SUCCESS);
			break;

		case LEER_DATO_DE_MEMORIA:
			log_info(logger, "LEER_DATO_DE_MEMORIA recibido.");

			t_peticion_memoria* solicitud_lectura = peticion_recibir(socket_cpu, LEER_DATO_DE_MEMORIA);

			log_info(logger_oblig, "PID: <%d> - Accion: <%s> - Direccion fisica: <%d> - Tamaño <%ld>",
				PID_solicitado, "LEER",solicitud_lectura->direccion_fisica, solicitud_lectura->tam_dato);

			void* ptr_base_del_dato = get_memoria(solicitud_lectura->direccion_fisica);

			log_info(logger, "DATO LEIDO: [%.*s]",(int)solicitud_lectura->tam_dato, (char*)ptr_base_del_dato);

			esperar_retardo();
			enviar_buffer(ptr_base_del_dato, solicitud_lectura->tam_dato, socket_cpu);
			enviar_status(SUCCESS, socket_cpu);
			log_info(logger, "Se leyó el dato de la memoria y se envió al CPU");
			break;

		case CONSULTAR_TABLA_DE_PAGINAS:
			log_info(logger, "CONSULTAR_TABLA_DE_PAGINAS recibido.");

			unsigned numero_pagina = recibir_cantidad(socket_cpu);
			log_info(logger, "Recibido. Número de página: [%d]", numero_pagina);

			unsigned numero_frame_consultado = get_frame_number_by_pagina(numero_pagina);
			esperar_retardo();
			
			log_info(logger_oblig, "PID: <%d> - Pagina: <%d> - Marco: <%d>", PID_solicitado, numero_pagina, numero_frame_consultado);

			enviar_cantidad(numero_frame_consultado, CONSULTAR_TABLA_DE_PAGINAS, socket_cpu);

			log_info(logger, "Se consultó la tabla de páginas y se envió al CPU");
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
		
			terminar_programa();
            break;        
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
            terminar_programa();
			break;
		}
        log_info(logger, "==============================================");
	}
}

void* gestionar_solicitudes_io(void* pthread_arg){
	int io_socket = *(int*)pthread_arg;
	free(pthread_arg);
	log_info(logger, "Se conectó un cliente IO. SOCKET: [%d]", io_socket);
	
	log_info(logger, "Esperando recibir operacion del IO...");
	while(io_socket != -1){

		op_code cod_op = recibir_operacion(io_socket);
		log_info(logger, "===================");
		log_info(logger, "Codigo recibido desde el IO: %d", cod_op);

		switch (cod_op) {
		case LEER_DATO_DE_MEMORIA:
			log_info(logger, "LEER_DATO_DE_MEMORIA recibido.");

			t_peticion_memoria* solicitud_lectura = peticion_recibir(io_socket, LEER_DATO_DE_MEMORIA);
			log_info(logger, "Recibido. Dirección_fisica: [%d], tam_dato: [%zu]", solicitud_lectura->direccion_fisica, solicitud_lectura->tam_dato);

			void* ptr_base_del_dato = get_memoria(solicitud_lectura->direccion_fisica);

			log_info(logger, "DATO LEIDO: [%.*s]", (int)solicitud_lectura->tam_dato, (char*)ptr_base_del_dato);

			esperar_retardo();
			enviar_buffer(ptr_base_del_dato, solicitud_lectura->tam_dato, io_socket);
			enviar_status(SUCCESS, io_socket);
			log_info(logger, "Se leyó el dato de la memoria y se envió a IO");
			break;

		case ESCRIBIR_DATO_EN_MEMORIA:
		log_info(logger, "ESCRIBIR_DATO_EN_MEMORIA recibido.");

		t_peticion_memoria* solicitud = peticion_recibir(io_socket, ESCRIBIR_DATO_EN_MEMORIA);
		log_info(logger, "Recibido. Dirección_fisica: [%d], Tam_Dato: [%zu]", solicitud->direccion_fisica, solicitud->tam_dato);

		set_memoria(solicitud->direccion_fisica, solicitud->dato, solicitud->tam_dato);

		esperar_retardo();
		enviar_status(SUCCESS, io_socket);
		log_info(logger, "Se escribió dato en memoria correctamente, envio: [%d]", SUCCESS);
		break;

		case -1:
			log_error(logger, "La IO se desconecto. Terminando servidor");
			close(io_socket);
			io_socket = -1;
			break;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata. CODIGO: [%d]", cod_op);
			break;
		}
	}
}

void terminar_programa()
{
    log_info(logger, "Memoria liberada correctamente");
    log_destroy(logger);
}


/**

Esta parte de la memoria será la encargada de obtener de los archivos de pseudo código las instrucciones y 
de devolverlas a pedido a la CPU.
Al momento de recibir la creación de un proceso, la memoria de instrucciones deberá leer el archivo de pseudocódigo indicado y
generar las estructuras que el grupo considere necesarias para poder devolver las instrucciones de a 1 a la CPU 
según ésta se las solicite por medio del Program Counter.
Ante cada petición se deberá esperar un tiempo determinado a modo de retardo en la obtención de la instrucción, y este tiempo,
 estará indicado en el archivo de configuración.


**/