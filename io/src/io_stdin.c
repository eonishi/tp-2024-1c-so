#include "../include/io_stdin.h"


char* iniciar_consola(uint32_t tamanio_maximo){
	char* leido = NULL;

	while(true){
		log_info(logger, "Ingrese máximo [%d] caracteres", tamanio_maximo);

		leido = readline("> ");
		log_info(logger, "Linea ingresada: %s", leido);

		if (string_length(leido) == tamanio_maximo) 
            return leido;

        else if (string_length(leido) > tamanio_maximo)
            log_warning(logger, "Se ha superado el tamanio máximo de: [%d] bytes", tamanio_maximo);
        else
            log_warning(logger, "Se ha ingresado menos de [%d] bytes", tamanio_maximo);
     }
}

static void controlar_peticion(){
     op_code status = recibir_operacion(memory_socket);
     if(status == SUCCESS){
        log_info(logger, "La operacion en memoria fue exitosa");
     }
     else{
        log_error(logger, "Hubo un problema con la operacion en memoria");
     }
}

// IO_STDIN_READ (Interfaz, Registro Dirección, Registro Tamaño):
void io_stdin() {
     log_info(logger, "Iniciando escucha STDIN");

     while (kernel_socket != -1){
        int cod_op = recibir_operacion(kernel_socket);
        log_info(logger, "Codigo de operacion: [%d]", cod_op);

        switch (cod_op){
            case EJECUTAR_INSTRUCCION_IO:
                solicitud_instruccion_io solicitud = recibir_instruccion_io(kernel_socket);
                int pid = solicitud.pid;
                t_list *peticiones_memoria = solicitud.peticiones_memoria;
                char** tokens_instr = solicitud.tokens;

                log_info(logger,"RECIBIDO EJECUTAR_INSTRUCCION_IO, pid: [%d]", pid);
                
                log_info(logger, "Instruccion recibida de Kernel [%s]", tokens_instr[0]);
                log_peticiones(peticiones_memoria);

                size_t tamanio_string = peticiones_tam_total(peticiones_memoria);

                // Leer por consola
                char* valor_ingresado = iniciar_consola(tamanio_string);
                log_info(logger, "Valor ingresado: [%s]", valor_ingresado);

                // Distribuir dato entre todas las peticiones
                peticiones_distribuir_dato(peticiones_memoria, (void*)valor_ingresado, tamanio_string);

                //Enviar peticiones a memoria
                for (int i = 0; i < list_size(peticiones_memoria); i++){
                    t_peticion_memoria* peticion = list_get(peticiones_memoria, i);
                    peticion_escritura_enviar(peticion, memory_socket);
                    controlar_peticion();
                }
                free(valor_ingresado);

                // devolver status
                enviar_status(FIN_EJECUCION_IO, kernel_socket);
                log_info(logger, "Instruccion IO ejecutada");
                break;

            case -1:
                log_error(logger, "Se desconecto el kernel");
                close(kernel_socket);
                kernel_socket = -1;
                close(memory_socket);
                memory_socket = -1;
                exit(EXIT_FAILURE);
            default:
                log_error(logger, "Codigo de operacion no reconocido");
                break;
        }
    }
}

