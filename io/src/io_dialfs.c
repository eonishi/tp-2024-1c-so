#include "../include/io_dialfs.h"



void io_dialfs() {
    inicializar_filesystem();
    
    imprimir_bitmap();
    
    log_info(logger, "Hola, soy IO DIALFS :)");

    int cod_op = 1; 
    switch (cod_op)
    {
		case 1:
            log_info(logger, "IO_FS_CREATE");
            crear_archivo("notas.txt");
        break;
    }
}

