#include "../include/fs_blocks.h"

static void *BLOQUES;
static t_bitarray* BLOQ_BITMAP;
int tam_filesystem = 0;


void inicializar_bloques(){
    tam_filesystem = config.block_count * config.block_size;

    inicializar_archivo_bloques();
    inicializar_archivo_bitmap();
}

void inicializar_archivo_bloques(){
    int fd = fs_open("bloques.dat", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

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

void inicializar_archivo_bitmap(){
    int fd = fs_open("bitmap.dat", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    if (fd == -1) {
        log_error(logger, "Error al abrir/crear el archivo bitmap.dat");
        exit(EXIT_FAILURE);
    }

    if (archivo_esta_vacio(fd)) {
        log_info(logger, "El bitmap está vacio");
        if (!inicializar_bitmap_en_archivo(fd)) {
            log_error(logger, "Error al intentar inicializar el bitmap de bloques en el archivo");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }
    
    int bitmap_file_size = sizeof(t_bitarray) + config.block_count/8; // 32 bloques -> 32 bits -> ocupa 4 bytes 

    ftruncate(fd, bitmap_file_size);
    
    void* mapped_file = (t_bitarray*) enlazar_archivo(fd, bitmap_file_size);    

    if(!mapped_file){
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
    // Con MAP_SHARED se sincroniza automáticamente
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

void asignar_bloques_bitmap_por_rango(int desde, int hasta){
    for(int i = desde; i < hasta; i++ ){
        bitarray_set_bit(BLOQ_BITMAP, i);
    }    
}

void liberar_bloques_bitmap_por_rango(int desde, int hasta){
    for(int i = desde; i <= hasta; i++){
        bitarray_clean_bit(BLOQ_BITMAP, i);
    }
}

void eliminar_bloques_ocupados_por_archivo(char* nombre){
    fcb* fcb = obtener_fcb_por_nombre(nombre);
    int bloque_inicial = config_get_int_value(fcb->config, "BLOQUE_INICIAL");
    int tam_archivo = config_get_int_value(fcb->config, "TAMANIO_ARCHIVO");
    int bloques_ocupados = calcular_bloques_necesarios(tam_archivo);

    if(bloques_ocupados == 1){
        bitarray_clean_bit(BLOQ_BITMAP, bloque_inicial);
    }
    else{
        int bloque_final = (bloque_inicial + bloques_ocupados) - 1;

        liberar_bloques_bitmap_por_rango(bloque_inicial, bloque_final);
    }
}

void liberar_bitmap_de_bloques(){
    for(int i = 0; i <= config.block_count; i++){
        bitarray_clean_bit(BLOQ_BITMAP, i);
    }
}

bool esta_bloque_ocupado(int index){
    return bitarray_test_bit(BLOQ_BITMAP, index);
}

int calcular_bloques_necesarios(int size){
    if(size == 0)
        return 1;

    return (size + config.block_size - 1) / config.block_size;// Redondeo hacia arriba
}


void reubicar_archivo_desde_fcb(fcb* fcb){
    int bloque_inicial = asignar_bloque();
    int tamanio_archivo = config_get_int_value(fcb->config, "TAMANIO_ARCHIVO");
    int bloques_a_ocupar = calcular_bloques_necesarios(tamanio_archivo);

    asignar_bloques_bitmap_por_rango(bloque_inicial, bloques_a_ocupar);
    set_campo_de_archivo("BLOQUE_INICIAL", bloque_inicial, fcb->config);
    config_save(fcb->config);
}

void compactar() {
    log_info(logger, "======== Se ejecuta compactación =========");
    // Crear un espacio temporal para almacenar datos mientras se compactan
    void *nuevo_espacio = calloc(tam_filesystem, 1);
    if (nuevo_espacio == NULL) {
        log_error(logger, "Error al asignar memoria para la compactación.");
        exit(EXIT_FAILURE);
    }

    // Liberar el bitmap de bloques y los bloques antiguos
    liberar_bitmap_de_bloques();

    // Recorrer los FCBs y copiar los datos a las nuevas ubicaciones
    for (int i = 0; i < cantidad_de_fcbs(); i++) {
        fcb* fcb = obtener_fcb(i);
        int bloque_inicial = config_get_int_value(fcb->config, "BLOQUE_INICIAL");
        int tamanio_archivo = config_get_int_value(fcb->config, "TAMANIO_ARCHIVO");

        // Calcular los bloques necesarios para el archivo
        int bloques_a_ocupar = calcular_bloques_necesarios(tamanio_archivo);

        // Leer los datos desde los bloques antiguos
        int byte_inicial = calcular_byte_inicial_de_bloque(bloque_inicial);
        void *datos_leidos = malloc(tamanio_archivo);
        if (datos_leidos == NULL) {
            log_error(logger, "Error al asignar memoria para leer datos del archivo.");
            free(nuevo_espacio);
            exit(EXIT_FAILURE);
        }

        memcpy(datos_leidos, (char*)BLOQUES + byte_inicial, tamanio_archivo);

        // Encontrar nuevos bloques para los datos
        int nuevo_bloque_inicial = asignar_bloque();
        int nuevo_byte_inicial = calcular_byte_inicial_de_bloque(nuevo_bloque_inicial);

        // Escribir los datos en los nuevos bloques
        memcpy((char*)nuevo_espacio + nuevo_byte_inicial, datos_leidos, tamanio_archivo);

        // Liberar los bloques antiguos
        asignar_bloques_bitmap_por_rango(nuevo_bloque_inicial, nuevo_bloque_inicial + bloques_a_ocupar - 1);

        // Actualizar el FCB con los nuevos bloques
        set_campo_de_archivo("BLOQUE_INICIAL", nuevo_bloque_inicial, fcb->config);
        config_save(fcb->config);

        free(datos_leidos);
    }

    // Guardar los datos compactados en el nuevo espacio
    memcpy(BLOQUES, nuevo_espacio, tam_filesystem);

    // Liberar el espacio temporal
    free(nuevo_espacio);

    // Esperar el retraso de compactación
    usleep(config.retraso_compatacion * 1000);
}


int calcular_byte_inicial_de_bloque(int bloque_inicial){
    return (bloque_inicial != 0) ? bloque_inicial * config.block_size : 0;
}


bool escribir_datos_en_bloques(int posicion_inicial, void* datos, int tam_total){
    if (posicion_inicial + tam_total > config.block_count * config.block_size) {
        log_error(logger, "Error: Se intenta escribir fuera de los límites de los bloques");
        return false;
    }

    memcpy((char*)BLOQUES + posicion_inicial, datos, tam_total);

    return true;
}

bool leer_datos_en_bloques(int posicion_inicial, int tam_total, void** datos_leidos) {
    if (posicion_inicial + tam_total > config.block_count * config.block_size) {
        log_error(logger, "Error: Se intenta leer fuera de los límites de los bloques");
        return false;
    }

    *datos_leidos = malloc(tam_total);
    if (*datos_leidos == NULL) {
        log_error(logger, "Error: No se pudo asignar memoria para los datos leídos");
        return false;
    }

    // Copiar los datos desde BLOQUES a datos_leidos
    memcpy(*datos_leidos, (char*)BLOQUES + posicion_inicial, tam_total);

    return true;
}

// -----------------------------------------------//