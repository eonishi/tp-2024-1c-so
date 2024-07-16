#include "../include/filesystem.h"

static void *BLOQUES;
static t_bitarray* BLOQ_BITMAP;
int tam_filesystem = 0;
t_list* fcb_list;

// ---- Init FS --------------------------//
bool inicializar_filesystem(){
    tam_filesystem = config.block_count * config.block_size;
    fcb_list = list_create();

    inicializar_bloques();
    inicializar_bitmap();

    return true;
}

void inicializar_bloques(){
    int fd = open("bloques.dat", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    if (fd == -1) {
        log_error(logger, "Error al abrir/crear el archivo");
        exit(EXIT_FAILURE);
    }

    if (archivo_esta_vacio(fd)) {
        if (!inicializar_bloques_en_archivo(fd)) {
            log_error(logger, "Error al intentar inicializar los bloques en el archivo");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    ftruncate(fd, tam_filesystem);
    
    BLOQUES = enlazar_archivo(fd, tam_filesystem);

    if(!BLOQUES){
        log_error(logger, "No se pudo enlazar el archivo");        
    }

    close(fd);
}

void inicializar_bitmap(){
    log_info(logger, "inicializar_bitmap()...");

    int fd = open("bitmap.dat", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    if (fd == -1) {
        log_error(logger, "Error al abrir/crear el archivo bitmap.dat");
        exit(EXIT_FAILURE);
    }

    if (archivo_esta_vacio(fd)) {
        log_info(logger, "El bitmap está vacio");
        if (!inicializar_bitmap_en_archivo(fd)) {
            log_error(logger, "Error al intentar inicializar los bloques en el archivo");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }
    
    int bitmap_file_size = sizeof(t_bitarray) + config.block_count/8; // 32 bloques -> 32 bits -> ocupa 4 bytes 

    log_info(logger, "bitmap_file_size = [%d]", bitmap_file_size);
    ftruncate(fd, bitmap_file_size);
    
    void* mapped_file = (t_bitarray*) enlazar_archivo(fd, bitmap_file_size);    

    if(!BLOQ_BITMAP){
        log_error(logger, "No se pudo enlazar el archivo");        
    }

    //+-------------------------------+----------------+
    //|  t_bitarray (16 bytes)        | bitarray (4 bytes) |
    //+-------------------------------+----------------+
    t_bitarray *bloq_bitmap = (t_bitarray *)mapped_file; // Primero convertimos el puntero generico a un puntero t_bitarray
    bloq_bitmap->bitarray = (char *)mapped_file + sizeof(t_bitarray); // Convertirmos el puntero generico a (char *) para poder "saltear" los primeros 16 bytes
    BLOQ_BITMAP = bloq_bitmap;

    close(fd);
}

bool archivo_esta_vacio(int fd) {
    struct stat sb;

    // Obtener el tamaño del archivo
    if (fstat(fd, &sb) == -1) {
        perror("Error al obtener el tamaño del archivo");
        close(fd); // Cerrar el archivo
        exit(EXIT_FAILURE);
    }

    // Verificar el tamaño del archivo
    if (sb.st_size == 0)
        return true;

    return false;
}

bool inicializar_bloques_en_archivo(int fd){
        // Inicialización de bloques
        void *init_bloques = calloc(config.block_size, config.block_count);
        if (init_bloques == NULL) {
            perror("Error al asignar memoria para bloques");
            close(fd);
            exit(EXIT_FAILURE);
        }

        // Escribir en el archivo usando write
        ssize_t written = write(fd, init_bloques, tam_filesystem);
        free(init_bloques); // Liberar la memoria asignada

        if (written != tam_filesystem) {
           return false;
        }

        return true;
}

bool inicializar_bitmap_en_archivo(int fd) {
    // Inicialización de bloques
    int cantidad_bits_en_bytes = config.block_count / 8; // 1 byte = 8 bits
    char *bitmap = calloc(cantidad_bits_en_bytes, sizeof(char));
    
    t_bitarray *bloq_bitmap = malloc(sizeof(t_bitarray));
    bloq_bitmap = bitarray_create_with_mode(bitmap, cantidad_bits_en_bytes, MSB_FIRST);
    BLOQ_BITMAP = bloq_bitmap;
    imprimir_bitmap();
    log_info(logger, "Accediendo a BLOQ_BITMAP->size: [%d]", BLOQ_BITMAP->size);
    log_info(logger, "Accediendo a BLOQ_BITMAP->mode: [%d]", BLOQ_BITMAP->mode);

    // Escribir la estructura completa en el archivo
    ssize_t written = write(fd, bloq_bitmap, sizeof(t_bitarray));
    if (written != sizeof(t_bitarray)) {
        free(bitmap); // Liberar la memoria asignada
        return false;
    }
    // Escribir el bitarray
    written = write(fd, bitmap, cantidad_bits_en_bytes);
    if (written != cantidad_bits_en_bytes) {
        free(bitmap); // Liberar la memoria asignada
        return false;
    }

    free(bitmap); // Liberar la memoria asignada
    return true;
}

void *enlazar_archivo(int fd, int tam_archivo) {    
    void *map = mmap(NULL, tam_archivo, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (map == MAP_FAILED) {
        perror("Error al mapear el archivo");
        return NULL;
    }

    return map;
}
// -----------------------------------------------//

// ---- Utiles --------------------------//
void imprimir_bitmap() {
    log_info(logger, "Bitmap de bloques: ");

    char* bitmap = BLOQ_BITMAP->bitarray;

    printf("[");
    bool primero = true;
    for (int i = 0; i < config.block_count; i++) {
        if (!primero) {
                printf(",");
            }
            primero = false;
            bool valor_del_bit = bitarray_test_bit(BLOQ_BITMAP, i);
            printf("%d", valor_del_bit);
    }
    printf("]\n");
}
// -----------------------------------------------//

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

    list_add(fcb_list, new_fcb);
    // --

    return true;
}

int asignar_bloque(){
    int bloque = buscar_bloque_libre();

    if(bloque == -1){
        return -1;
    }

    bitarray_set_bit(BLOQ_BITMAP, bloque);

    return bloque;
}

int buscar_bloque_libre(){
    for (size_t i = 0; i < config.block_count; i++){
        if (!bitarray_test_bit(BLOQ_BITMAP, i)){
            log_info(logger, "Bloque disponible [%d]", i);
            return i;
        }
    }
    log_error(logger, "No hay bloques disponibles");
    return -1;
}
// -----------------------------------------------//

char* nombre_aux;
bool condicion_por_nombre(void* file_control_block){
    return strcmp(((fcb*)file_control_block)->nombre, nombre_aux) == 0;
}

fcb* obtener_fcb_por_nombre(char* nombre){
    nombre_aux = nombre;

    return (fcb*) list_find(fcb_list, condicion_por_nombre);
}

void eliminar_fcb_por_nombre(char* nombre){
    nombre_aux = nombre;
    
    list_remove_by_condition(fcb_list, condicion_por_nombre);
}

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

    log_info(logger, "bloques_a_ocupar: [%d]", bloques_a_ocupar);
    log_info(logger, "bloque_inicial: [%d]", bloque_inicial);
    log_info(logger, "tam_archivo: [%d]", tam_archivo);
    log_info(logger, "bloques_actuales_ocupados: [%d]", bloques_actuales_ocupados);    

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


int calcular_bloques_a_ocupar(int size){
    if(size == 0)
        return 1;

    return (size + config.block_size - 1) / config.block_size;// Redondeo hacia arriba
}

void asignar_bloques_bitmap_por_rango(int desde, int hasta){
    for(int i = desde; i < hasta; i++ ){
        bitarray_set_bit(BLOQ_BITMAP, i);
    }    
}

bool hay_bloques_contiguos_para_extender(int bloque_inicial, int cantidad_bloques_actuales, int bloques_necesarios){
    // Ejemplo: 0 (bloque_inicial) + 2 (cantidad_bloques_actuales) -> Ocupa el 0 y 1, y el 2 es el siguiente;
    int siguiente_bloque = (bloque_inicial + cantidad_bloques_actuales); 

    for(int i = siguiente_bloque ; i < (siguiente_bloque + bloques_necesarios); i++){
        if(bitarray_test_bit(BLOQ_BITMAP, i)) // Si alguno está ocupado, retorno false;
            return false;
    }

    return true;
}

bool hay_bloques_libres_suficientes(int bloques_necesarios){
    int libres = 0;
    for(int i = 0 ; i < (config.block_count); i++){
        if(!bitarray_test_bit(BLOQ_BITMAP, i)) 
            libres++;
        if(libres == bloques_necesarios){
            return true;
        }
    }

 
    return false;
}

void liberar_bloques_bitmap_por_rango(int desde, int hasta){
    for(int i = desde; i <= hasta; i++){
        bitarray_clean_bit(BLOQ_BITMAP, i);
    }
}

void liberar_bitmap_de_bloques(){
    for(int i = 0; i <= config.block_count; i++){
        bitarray_clean_bit(BLOQ_BITMAP, i);
    }
}


void compactar(){
    log_info(logger, "liberar_bitmap_de_bloques...");
    liberar_bitmap_de_bloques();
    // Liberar bloques??

    // Volvemos a insertar secuencialmente los archivos
    for(int i = 0; i < fcb_list->elements_count; i++){
        fcb* fcb = list_get(fcb_list, i);

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

void set_campo_de_archivo(char* campo , int valor, t_config* config_loader){
    char* valor_char[5];
    sprintf(valor_char, "%d", valor);
    config_set_value(config_loader, campo, valor_char);
    config_save(config_loader);
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