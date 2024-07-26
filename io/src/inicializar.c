#include "../include/inicializar.h"

t_log *logger;
t_log *logger_oblig;

// Estas funciones iniciadoras se pueden refactorizar en una sola, 
// pero nose si vaya a necesitar ajusten especificos para cada tipo de interfaz
static void iniciar_io_generica(char* nombre_interfaz){
    int operaciones[1] = {IO_GEN_SLEEP};
    int* ptr_op = operaciones;
    
    int conexion_valida = conectar_al_kernel(nombre_interfaz, GENERICA, ptr_op);
    if(conexion_valida) io_generica();
    else exit(EXIT_FAILURE);
}

static void iniciar_io_stdin(char* nombre_interfaz){
    int operaciones[1] = {IO_STDIN_READ};
    int* ptr_op = operaciones;

    int conexion_valida = conectar_al_kernel(nombre_interfaz, STDIN, ptr_op);
    if(conexion_valida) io_stdin();
    else exit(EXIT_FAILURE);
}

static void iniciar_io_stdout(char* nombre_interfaz){
    int operaciones[1] = {IO_STDOUT_WRITE};
    int* ptr_op = operaciones;

    int conexion_valida = conectar_al_kernel(nombre_interfaz, STDOUT, ptr_op);
    if(conexion_valida) io_stdout();
    else exit(EXIT_FAILURE);
}

static void iniciar_io_dialfs(char* nombre_interfaz){
    int operaciones[5] = {IO_FS_CREATE, IO_FS_DELETE, IO_FS_TRUNCATE, IO_FS_WRITE, IO_FS_READ};
    int* ptr_op = operaciones;

    int conexion_valida = conectar_al_kernel(nombre_interfaz, DIALFS, ptr_op);
    if(conexion_valida) io_dialfs();
    else exit(EXIT_FAILURE);
}

void inicializar_io(char* nombre_interfaz, char* path_config){
    // Inicializacion comun entre todas los tipos de interfaces
    char* logger_name = string_from_format("IO_%s", nombre_interfaz);
    logger = iniciar_logger("io.log", logger_name, 1);
    logger_oblig = iniciar_logger("logs_obligatorios.log", logger_name, 1);

    inicializar_configuracion(path_config);
    crear_conexion_kernel();

    // Inicializacion especifica dependiendo la interfaz
    if(string_equals_ignore_case(config.interfaz_tipo, "GENERICA")){
        iniciar_io_generica(nombre_interfaz);
        log_info(logger, "Interfaz generica inicializada");
    }
    else if(string_equals_ignore_case(config.interfaz_tipo, "STDIN")){
        crear_conexion_memoria();
        iniciar_io_stdin(nombre_interfaz);
        log_info(logger, "Interfaz STDIN inicializada");
    }
    else if(string_equals_ignore_case(config.interfaz_tipo, "STDOUT")){
        crear_conexion_memoria();
        iniciar_io_stdout(nombre_interfaz);
        log_info(logger, "Interfaz STDOUT inicializada");
    }
    else if(string_equals_ignore_case(config.interfaz_tipo, "DIALFS")){
        crear_conexion_memoria();

        if(!inicializar_filesystem()) exit(EXIT_FAILURE);
        
        iniciar_io_dialfs(nombre_interfaz);
        log_info(logger, "Interfaz DialFS inicializada");
    }
    else{
        log_error(logger, "Interfaz no reconocida");
        exit(EXIT_FAILURE);
    }
}
