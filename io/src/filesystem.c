#include "../include/filesystem.h"

// ---- Init FS --------------------------//
bool inicializar_filesystem(){
    inicializar_lista_fcb();
    inicializar_bloques();

    return true;
}

// ---- CREATE --------------------------//
bool crear_archivo(char* nombre){
    int fd = open(nombre, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

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

    // Agregar archivo a lista_fcb
    fcb* new_fcb = (fcb*)malloc(sizeof(fcb));
    new_fcb->nombre = nombre;
    new_fcb->config = config_loader;

    crear_fcb(new_fcb);
    // --

    return true;
}
// -----------------------------------------------//

// ---- Truncate --------------------------//
bool truncar_archivo(char* nombre, int new_size){
    log_info(logger, "Entro a funcion truncar_archivo... size:[%d]", new_size);

    int fd = open(nombre, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

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
    int bloques_actuales_ocupados = (tam_archivo == 0) ? 1 : tam_archivo/8;

    if(new_size > tam_archivo){
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

            bloque_inicial = config_get_int_value(config_loader, "BLOQUE_INICIAL");
        }
    }

    ftruncate(fd, new_size);

    // Actualizamos properties del archivo
    set_campo_de_archivo("TAMANIO_ARCHIVO", new_size, config_loader);
    set_campo_de_archivo("BLOQUE_INICIAL", bloque_inicial, config_loader);
    // Actualizamos bitmap
    asignar_bloques_bitmap_por_rango(bloque_inicial, bloques_a_ocupar);

    log_info(logger, "Final truncate: TAMANIO_ARCHIVO: [%d]", config_get_int_value(config_loader, "TAMANIO_ARCHIVO"));

    close(fd);

    return true;
}



bool hay_bloques_contiguos_para_extender(int bloque_inicial, int cantidad_bloques_actuales, int bloques_necesarios){
    // Ejemplo: 0 (bloque_inicial) + 2 (cantidad_bloques_actuales) -> Ocupa el 0 y 1, y el 2 es el siguiente;
    int siguiente_bloque = (bloque_inicial + cantidad_bloques_actuales); 

    for(int i = siguiente_bloque ; i < (siguiente_bloque + bloques_necesarios); i++){
        if(esta_bloque_ocupado(i)) // Si alguno está ocupado, retorno false;
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
    log_info(logger, "liberar_bitmap_de_bloques...");
    liberar_bitmap_de_bloques();
    // Liberar bloques??

    // Volvemos a insertar secuencialmente los archivos
    for(int i = 0; i < cantidad_de_fcbs(); i++){
        fcb* fcb = obtener_fcb(i);

        log_info(logger, "Volviendo a guardar archivo: [%s]", fcb->nombre);
        reubicar_archivo_desde_fcb(fcb);
    }
    // Reorganizar bloques?    
}

void reubicar_archivo_desde_fcb(fcb* fcb){
    int bloque_inicial = asignar_bloque();
    int tamanio_archivo = config_get_int_value(fcb->config, "TAMANIO_ARCHIVO");
    int bloques_a_ocupar = calcular_bloques_a_ocupar(tamanio_archivo);

    asignar_bloques_bitmap_por_rango(bloque_inicial, bloques_a_ocupar);
    set_campo_de_archivo("BLOQUE_INICIAL", bloque_inicial, fcb->config);
    config_save(fcb->config);
}

// -----------------------------------------------//

// Sincronizar los cambios al archivo // TODO esto se puede sacar me parece
//if (msync(map, tam_archivo, MS_SYNC) == -1) {
    //  log_info(logger, "Error al sincronizar el archivo");
    //return NULL;
//}

// Desmapear el archivo
//if (munmap(BLOQUES, tam_filesystem) == -1) {
    //  log_info(logger, "Error al desmapear el archivo");
//}