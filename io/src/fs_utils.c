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
    char* path = concat_chars(config.path_base_dialfs, filename);
    log_info(logger, "Abriendo archivo: [%s]", path);

    int fd = open(path, flags, mode);

    // free(path);

    if (fd == -1) {
        log_error(logger, "Error al abrir el archivo: [%s]", filename);
    }

    return fd;
}

