#include "include/io.h"

int main()
{
    logger = iniciar_logger("io.log", "IO");
    log_info(logger, "Logger IO Iniciado");

    config = inicializar_config();

	if(!cargar_configuracion(config) || !generar_conexiones()){
		log_error(logger, "No se puede iniciar. Se cierra el programa");
		return EXIT_FAILURE;
	}

    iniciar_escucha_kernel();

    terminar_programa(socket_kernel);

    return 0;
}

void iniciar_escucha_kernel(){
	t_list* lista;
	bool on = 1;
	while (on) {
        log_info(logger, "Esperando recibir operacion desde el Kernel...");
		int cod_op = recibir_operacion(socket_kernel);

		switch (cod_op) {	
            case EJECUTAR_INSTRUCCION_IO:
                log_info(logger, "Recibi EJECUTAR_INSTRUCCION_IO. CODIGO: %d", cod_op);
                char** instrucciones_tokenizadas = recibir_instruccion_io(socket_kernel);

                sleep(3);

                log_info(logger, "Pasaron 3 segundos, desbloqueamos");
                
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



void terminar_programa(int conexion_kernel)
{
    liberar_conexion(conexion_kernel);
    log_info(logger, "Memoria liberada correctamente");
    log_destroy(logger);
}

/*
Las interfaces de I/O pueden ser varias, en la realidad las conocemos como Teclados, Mouse, Discos, Monitores o hasta Impresoras.
Las mismas irán recibiendo desde Kernel distintas operaciones a realizar para determinado proceso,
y le irán dando aviso a dicho módulo una vez completadas.


*/