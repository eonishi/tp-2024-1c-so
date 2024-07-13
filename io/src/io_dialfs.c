#include "../include/io_dialfs.h"



void io_dialfs() {
    inicializar_filesystem();
    
    imprimir_bitmap();
    
    log_info(logger, "Hola, soy IO DIALFS :)");

    int cod_op = ; 
    switch (cod_op)
    {
		case 1:
            log_info(logger, "IO_FS_CREATE");

            if(!crear_archivo("notas.txt")){
                log_error(logger, "No se pudo crear el archivo");
            }

            imprimir_bitmap();
        break;
        case 2:
            log_info(logger, "IO_FS_TRUNCATE");
            truncar_archivo("notas.txt", 100);

            imprimir_bitmap();
        break;
    }
}

