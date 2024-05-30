#include "../include/inicializar.h"

t_log *logger;

static void iniciar_io_generica(char* nombre_interfaz){
    int operaciones[1] = {IO_GEN_SLEEP};
    int* ptr_op = operaciones;
    
    int conexion_valida = conectar_al_kernel(nombre_interfaz, GENERICA, ptr_op);
    if (conexion_valida){
        io_generica();
    }
    else{
        exit(EXIT_FAILURE);
    }
}

void inicializar_io(char* nombre_interfaz, char* path_config){
    // Inicializacion comun entre todas los tipos de interfaces
    logger = iniciar_logger("io.log", "IO");
    inicializar_configuracion(path_config);
    crear_conexion_kernel();
    
    // Inicializacion especifica dependiendo la interfaz
    if(string_equals_ignore_case(config.interfaz_tipo, "GENERICA")){
        // INICIALIZACION DE LA INTERFAZ GENERICA
        iniciar_io_generica(nombre_interfaz);
        log_info(logger, "Interfaz generica inicializada");
    }
    else if(string_equals_ignore_case(config.interfaz_tipo, "STDIN")){
        // INICIALIZACION DE LA INTERFAZ STDIN
    }
    else if(string_equals_ignore_case(config.interfaz_tipo, "STDOUT")){
        // INICIALIZACION DE LA INTERFAZ STDOUT
    }
    else if(string_equals_ignore_case(config.interfaz_tipo, "DIALFS")){
        // INICIALIZACION DE LA INTERFAZ DIALFS
    }
    else{
        log_error(logger, "Interfaz no reconocida");
        exit(EXIT_FAILURE);
    }
}
