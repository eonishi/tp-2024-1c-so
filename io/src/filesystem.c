#include "../include/filesystem.h"

bool inicializar_filesystem(){
    inicializar_lista_fcb();
    inicializar_bloques();

    return true;
}

bool crear_archivo(char* nombre){
    int fd = fs_open(nombre, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

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
    char* contenido_inicial = malloc(sizeof(char) * 100); // Hay que hacer free de esta variable
    sprintf(contenido_inicial, "BLOQUE_INICIAL=%d\nTAMANIO_ARCHIVO=0\n", bloque);
    ssize_t bytes_escritos = write(fd, contenido_inicial, string_length(contenido_inicial));

    if (bytes_escritos == -1) {
        log_error(logger, "Error al escribir en el archivo: [%s]", nombre);
        close(fd);
        return false;
    }

    close(fd);

    t_config* config_loader = config_create(fs_fullpath(nombre));

    if(config_loader == NULL) {
        log_error(logger, "No se encontro el archivo del filesystem: [%s]", nombre);
        exit(EXIT_FAILURE);
    }

    crear_fcb(nombre, config_loader);

    return true;
}

bool eliminar_archivo(char* nombre) {
    eliminar_bloques_ocupados_por_archivo(nombre);

    if (remove(fs_fullpath(nombre)) == 0) {      
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

    int bloques_a_ocupar = calcular_bloques_necesarios(new_size); 
    int bloque_inicial = config_get_int_value(config_loader, "BLOQUE_INICIAL");
    int tam_archivo = config_get_int_value(config_loader, "TAMANIO_ARCHIVO");
    int bloques_actuales_ocupados = (tam_archivo == 0) ? 1 : calcular_bloques_necesarios(tam_archivo);
    int bloques_necesarios = bloques_a_ocupar - bloques_actuales_ocupados;

    if(new_size > tam_archivo){        
        if(!hay_bloques_contiguos_para_extender(bloque_inicial, bloques_actuales_ocupados, bloques_necesarios)){
            log_info(logger, "No hay bloques contiguos suficientes.");

            if(!hay_bloques_libres_suficientes(bloques_necesarios)){
                log_error(logger, "No hay bloques libres suficientes.");

                close(fd);
                return false;
            }

            eliminar_fcb_por_nombre(nombre);
            insertar_fcb(file_control_block);

            compactar();        

            bloque_inicial = config_get_int_value(config_loader, "BLOQUE_INICIAL");
        }
    }
    else{
        // [1,1,1,1,1,1,1] -> Va del 0 al 6 y tiene 7 bloques ocupados
        // anterior_ultimo_bloque = 0 + 7 - 1 = 6 es el último bloque
        int anterior_ultimo_bloque = (bloque_inicial + bloques_actuales_ocupados) - 1;
        // [1,1,1,1,1,0,0] -> Va del 0 al 4 y tiene 5 bloques ocupados
        // 0 + 5 - 1
        int nuevo_ultimo_bloque = bloque_inicial + bloques_a_ocupar - 1;  

        // Se libera del bloque 5 al 6      
        liberar_bloques_bitmap_por_rango(nuevo_ultimo_bloque + 1, anterior_ultimo_bloque);
    }

    // Actualizamos properties del archivo

    set_campo_de_archivo("TAMANIO_ARCHIVO", new_size, config_loader);
    set_campo_de_archivo("BLOQUE_INICIAL", bloque_inicial, config_loader);
  
    // Actualizamos bitmap
    asignar_bloques_bitmap_por_rango(bloque_inicial, bloque_inicial + bloques_a_ocupar);

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



bool escribir_archivo(char*nombre, void* datos, int tam_total, int puntero_archivo){
    fcb* fcb = obtener_fcb_por_nombre(nombre);

    int bloque_inicial = config_get_int_value(fcb->config, "BLOQUE_INICIAL");

    int byte_inicial = calcular_byte_inicial_de_bloque(bloque_inicial);

    int posicion_inicial = byte_inicial + puntero_archivo;

    if(!escribir_datos_en_bloques(posicion_inicial, datos, tam_total)){
        log_error(logger, "No se pudo escribir el archivo.");

        return false;
    };

    return true;
}

bool leer_archivo(char*nombre, int puntero_archivo, int tam_a_leer, void** datos_leidos){
    fcb* fcb = obtener_fcb_por_nombre(nombre);

    int bloque_inicial = config_get_int_value(fcb->config, "BLOQUE_INICIAL");

    int byte_inicial = calcular_byte_inicial_de_bloque(bloque_inicial);
    int posicion_inicial = byte_inicial + puntero_archivo;

    if(!leer_datos_en_bloques(posicion_inicial, tam_a_leer, datos_leidos)){
        log_error(logger, "No se pudo leer el archivo.");

        return false;
    };

    log_info(logger, "resultado leer_datos_en_bloques: [%s]", *datos_leidos);

    return true;
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