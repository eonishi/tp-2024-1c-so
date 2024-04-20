#include "include/io.h"

int main()
{
    logger = iniciar_logger("io.log", "IO");
    log_info(logger, "Logger IO Iniciado");

    // Esta lógica se repite para cada IO que se quiera crear.
    inicializar_configuracion();
    int kernel_fd = crear_conexion_kernel();
    enviar_mensaje("0", kernel_fd);


    terminar_programa(kernel_fd);

    // esto a chequear
    //int memory_fd = crear_conexion_memory();
    //enviar_mensaje(0, memory_fd);

    return 0;
}

int crear_conexion_kernel()
{
    log_info(logger, "Creando conexión con CPU...");
    int conexion = crear_conexion(config.IP_KERNEL, config.PUERTO_KERNEL);
    log_info(logger, "Conexión creada. Socket: %i", conexion);

    return conexion;
}

int crear_conexion_memory()
{
    log_info(logger, "Creando conexión con CPU...");
    int conexion = crear_conexion(config.IP_MEMORIA, config.PUERTO_MEMORIA);
    log_info(logger, "Conexión creada. Socket: %i", conexion);

    return conexion;
}

void inicializar_configuracion()
{
    log_info(logger, "Inicializando configuración...");

    t_config *config_loader = config_create("io.config");

    config.IP_KERNEL = config_get_string_value(config_loader, "IP_KERNEL");
    config.PUERTO_KERNEL = config_get_string_value(config_loader, "PUERTO_KERNEL");

    config.IP_MEMORIA = config_get_string_value(config_loader, "IP_MEMORIA");
    config.PUERTO_MEMORIA = config_get_string_value(config_loader, "PUERTO_MEMORIA");

    log_info(logger, "Configuración iniciadada correctamente.");
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