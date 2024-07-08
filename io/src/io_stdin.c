#include "../include/io_stdin.h"


char* iniciar_consola(uint32_t tamanio_maximo){
	char* leido;
	leido = "void";

	while(true){
		log_info(logger, "Ingrese máximo: [%d]", tamanio_maximo);

		leido = readline("> ");
		log_info(logger, "Linea ingresada: %s", leido);

		if (strlen(leido) <= tamanio_maximo)
			return leido;
          else{
               log_info(logger, "Se ha superado el tamanio máximo de: [%d] bytes", tamanio_maximo);
          }
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
                log_info(logger, "Ejecutar instruccion IO");
                char** instruccion_tokenizada = recibir_instruccion_io(kernel_socket);

                char* tamanio = instruccion_tokenizada[3];
                char* endptr;

                log_info(logger, "Instrucción: [%s %s %s %s]", instruccion_tokenizada[0],instruccion_tokenizada[1], instruccion_tokenizada[2], instruccion_tokenizada[3]);
                
               
                uint32_t tamanio_int = (uint32_t) strtoul(tamanio, &endptr, 10);

                char* valor_ingresado = iniciar_consola(tamanio_int);

                log_info(logger, "Valor ingresado: [%s]", valor_ingresado);
                // devolver status
                enviar_status(FIN_EJECUCION_IO, kernel_socket);
                log_info(logger, "Instruccion IO ejecutada");
                break;
                case -1:
                log_error(logger, "Se desconecto el kernel");
                exit(EXIT_FAILURE);
            default:
                log_error(logger, "Codigo de operacion no reconocido");
                break;
        }
    }
}

