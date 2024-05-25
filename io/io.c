#include "include/io.h"

int main(int argc, char* argv[])
{
    char* nombre_interfaz;
    char* path_config;
  
    check_args(argc, argv, &nombre_interfaz, &path_config);
    inicializar_io(nombre_interfaz, path_config);

    return EXIT_SUCCESS;
}

void check_args(int argc, char* argv[], char** nombre_interfaz, char** path_config){
    if(argc < 2){
        log_error(logger, "Falta el nombre de la interfaz");
        exit(EXIT_FAILURE);
    }else{
        *nombre_interfaz = argv[1];
    }
    if(argc < 3){
        *path_config = "io.config";
    }else{
        *path_config = argv[2];
    }
}




/*
Las interfaces de I/O pueden ser varias, en la realidad las conocemos como Teclados, Mouse, Discos, Monitores o hasta Impresoras.
Las mismas irán recibiendo desde Kernel distintas operaciones a realizar para determinado proceso,
y le irán dando aviso a dicho módulo una vez completadas.


*/