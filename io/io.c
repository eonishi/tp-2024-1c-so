#include "include/io.h"

int main(){
    logger = iniciar_logger("io.log", "IO");

    log_info(logger, "Logger IO Iniciado");

    return 0;
}

/*
Las interfaces de I/O pueden ser varias, en la realidad las conocemos como Teclados, Mouse, Discos, Monitores o hasta Impresoras.
Las mismas irán recibiendo desde Kernel distintas operaciones a realizar para determinado proceso, 
y le irán dando aviso a dicho módulo una vez completadas.


*/