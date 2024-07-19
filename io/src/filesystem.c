#include "../include/filesystem.h"

bool inicializar_filesystem(){
    inicializar_lista_fcb();
    inicializar_bloques();

    return true;
}

bool crear_archivo(char* nombre){
    int fd = fs_open(nombre, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    //TODO: VER SI AGREGAR VALIDACION DE SI YA EXISTE EL ARCHIVO
    if (fd == -1) {
        log_error(logger, "Error al crear el archivo: [%s]", nombre);
        return false;
    }

    int bloque = asignar_bloque();

    if (bloque == -1){
        log_error(logger, "No se puedo asignar un bloque");
        close(fd);

        return false;
    }

    // Contenido inicial del archivo
    char* contenido_inicial[100];
    sprintf(contenido_inicial, "BLOQUE_INICIAL=%d\nTAMANIO_ARCHIVO=0\n", bloque);
    ssize_t bytes_escritos = write(fd, contenido_inicial, strlen(contenido_inicial));

    if (bytes_escritos == -1) {
        log_error(logger, "Error al escribir en el archivo: [%s]", nombre);
        close(fd);
        return false;
    }

    close(fd);

    t_config* config_loader = config_create(nombre);

    if(config_loader == NULL) {
        log_error(logger, "No se encontro el archivo: [%s]", nombre);
        exit(EXIT_FAILURE);
    }

    crear_fcb(nombre, config_loader);

    return true;
}

bool eliminar_archivo(char* nombre) {
    eliminar_bloques_ocupados_por_archivo(nombre);

    if (remove(nombre) == 0) {      
        eliminar_fcb_por_nombre(nombre);

        return true; 
    } else {
        log_error(logger, "Error al eliminar el archivo");

        return false; // Falló al intentar eliminar el archivo
    }
}

bool truncar_archivo(char* nombre, int new_size){
    int fd = fs_open(nombre, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    if (fd == -1) {
        log_error(logger, "Error al leer el archivo: [%s]", nombre);
        return false;
    }

    fcb* file_control_block = obtener_fcb_por_nombre(nombre);

    t_config* config_loader = file_control_block->config;

    if(config_loader == NULL) {
        log_error(logger, "No se encontro el archivo: [%s]", nombre);
        exit(EXIT_FAILURE);
    }

    int bloques_a_ocupar = calcular_bloques_a_ocupar(new_size); 
    int bloque_inicial = config_get_int_value(config_loader, "BLOQUE_INICIAL");
    int tam_archivo = config_get_int_value(config_loader, "TAMANIO_ARCHIVO");
    
    if(new_size > tam_archivo){
        int bloques_actuales_ocupados = (tam_archivo == 0) ? 1 : tam_archivo/8;
        int bloques_necesarios = bloques_a_ocupar - bloques_actuales_ocupados;

        if(!hay_bloques_contiguos_para_extender(bloque_inicial, bloques_actuales_ocupados, bloques_necesarios)){
            log_info(logger, "No hay bloques contiguos suficientes.");

            if(!hay_bloques_libres_suficientes(bloques_necesarios)){
                log_error(logger, "No hay bloques libres suficientes.");

                close(fd);
                return false;
            }

            eliminar_fcb_por_nombre(nombre);
            
            compactar();

            reubicar_archivo_desde_fcb(file_control_block);
            insertar_fcb(file_control_block);


            bloque_inicial = config_get_int_value(config_loader, "BLOQUE_INICIAL");
        }
    }

    ftruncate(fd, new_size);

    // Actualizamos properties del archivo
    set_campo_de_archivo("TAMANIO_ARCHIVO", new_size, config_loader);
    set_campo_de_archivo("BLOQUE_INICIAL", bloque_inicial, config_loader);
    // Actualizamos bitmap
    asignar_bloques_bitmap_por_rango(bloque_inicial, bloques_a_ocupar);

    close(fd);

    return true;
}

bool hay_bloques_contiguos_para_extender(int bloque_inicial, int cantidad_bloques_actuales, int bloques_necesarios){
    // Ejemplo: 0 (bloque_inicial) + 2 (cantidad_bloques_actuales) -> Ocupa el 0 y 1, y el 2 es el siguiente;
    int bloque_siguiente = (bloque_inicial + cantidad_bloques_actuales); 

    for(int i = bloque_siguiente ; i < (bloque_siguiente + bloques_necesarios); i++){
        if(esta_bloque_ocupado(i)) 
            return false;
    }

    return true;
}

bool hay_bloques_libres_suficientes(int bloques_necesarios){
    int libres = 0;
    for(int i = 0 ; i < (config.block_count); i++){
        if(!esta_bloque_ocupado(i)) 
            libres++;
        if(libres == bloques_necesarios){
            return true;
        }
    }

 
    return false;
}

void compactar(){
    liberar_bitmap_de_bloques();
    // TODO: Sera necesario Liberar bloques??
    // Volvemos a insertar secuencialmente los archivos
    for(int i = 0; i < cantidad_de_fcbs(); i++){
        fcb* fcb = obtener_fcb(i);

        reubicar_archivo_desde_fcb(fcb);
    }
    // TODO: Sera necesario reorganizar bloques también??
}

// -----------------------------------------------//

// Sincronizar los cambios al archivo // 
//if (msync(map, tam_archivo, MS_SYNC) == -1) {
    //  log_info(logger, "Error al sincronizar el archivo");
    //return NULL;
//}

// TODO: agregar desmapear...
// Desmapear el archivo
//if (munmap(BLOQUES, tam_filesystem) == -1) {
    //  log_info(logger, "Error al desmapear el archivo");
//}