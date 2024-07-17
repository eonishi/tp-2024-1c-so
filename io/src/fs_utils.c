#include "../include/fs_utils.h"

char* concat_chars(char* str1, char* str2){
    size_t length = strlen(str1) + strlen(str2) + 1;

    char* result = (char*)malloc(length);
    if (result == NULL) {
        perror("No se pudo asignar memoria");
        return EXIT_FAILURE;
    }

    strcpy(result, str1);
    strcat(result, str2);

    return result;
}

int fs_open(const char* filename, int flags, mode_t mode){
    // TODO hacer funcionar fullpath home/utnso/dialfs
    char* path = concat_chars(config.path_base_dialfs, filename);
    log_info(logger, "Abriendo archivo: [%s]", filename);

    int fd = open(filename, flags, mode);

    // free(path);

    if (fd == -1) {
        log_error(logger, "Error al abrir el archivo: [%s]", filename);
    }

    return fd;
}

DIR* fs_opendir (){
    // TODO: config.path_base_dialfs
    DIR* dir = opendir(".");

    if (dir == NULL) {
        perror("No se puede abrir el directorio");
        return;
    }

    return dir;
}

