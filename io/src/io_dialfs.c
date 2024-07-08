#include "../include/io_dialfs.h"

static void *BLOQUES;
static t_bitarray* BLOQ_BITMAP;
static size_t size_bitmap;
int tam_filesystem = 0;

void prueba_escribir(){
    // Puedes acceder y modificar los datos a través del puntero `map`
    // Por ejemplo, puedes imprimir los primeros bytes del archivo
    printf("Primeros bytes del archivo: %.*s\n", tam_filesystem, (char *)BLOQUES);
    // Modificar el contenido del archivo (por ejemplo, escribir "Hola" en los primeros bytes)
    const char *message = "Hola";
    snprintf((char *)BLOQUES, tam_filesystem, "%s", message);
}

void io_dialfs() {
    tam_filesystem = config.block_count * config.block_size;

    inicializar_bloques();
    inicializar_bitmap();

    log_info(logger, "Hola, soy IO DIALFS :)");
}

void inicializar_bloques(){
    int fd = open("bloques.dat", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    if (fd == -1) {
        log_info(logger, "Error al abrir/crear el archivo");
        exit(EXIT_FAILURE);
    }

    if (archivo_esta_vacio(fd)) {
        if (!inicializar_bloques_en_archivo(fd)) {
            log_info(logger, "Error al intentar inicializar los bloques en el archivo");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }
    
    BLOQUES = enlazar_archivo(fd);

    // Sincronizar los cambios al archivo
    if (msync(BLOQUES, tam_filesystem, MS_SYNC) == -1) {
        log_info(logger, "Error al sincronizar el archivo");
    }

    // Desmapear el archivo
    if (munmap(BLOQUES, tam_filesystem) == -1) {
        log_info(logger, "Error al desmapear el archivo");
    }

    close(fd);
}

void inicializar_bitmap(){
    int cantidad_bits_en_bytes = config.block_count/8; // 1 byte = 8 bits
    char *bitmap = calloc(cantidad_bits_en_bytes, sizeof(char));
    BLOQ_BITMAP = bitarray_create_with_mode(bitmap, cantidad_bits_en_bytes, MSB_FIRST);
    size_bitmap = bitarray_get_max_bit(BLOQ_BITMAP);

    log_info(logger, "Bitmap de bloques creado con %d bits", size_bitmap);
}

bool archivo_esta_vacio(FILE* fd){
    struct stat sb;
    // Obtener el tamaño del archivo
    if (fstat(fd, &sb) == -1) {
        perror("Error al obtener el tamaño del archivo");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Verificar el tamaño del archivo
    if (sb.st_size == 0)
        return true;

    return false;
}

bool inicializar_bloques_en_archivo(FILE* fd){
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


void *enlazar_archivo(FILE *fd) {
    void *map = mmap(NULL, tam_filesystem, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (map == MAP_FAILED) {
        perror("Error al mapear el archivo");
        return NULL;
    }

    return map;
}