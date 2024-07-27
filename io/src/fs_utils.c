#include "../include/fs_utils.h"

char* fs_fullpath(const char* filename) {
    char* fs_path = config.path_base_dialfs;
    size_t len1 = strlen(fs_path);
    size_t len2 = strlen(filename);
    char* result = malloc(len1 + len2 + 2); // +2 para '/' y '\0'

    if (!result) {
        return NULL; // Manejar error de asignación
    }

    strcpy(result, fs_path);

    // Agregar '/' si no está presente al final de str1
    if (fs_path[len1 - 1] != '/') {
        result[len1] = '/';
        strcpy(result + len1 + 1, filename);
    } else {
        strcpy(result + len1, filename);
    }

    return result;
}


int fs_open(const char* filename, int flags, mode_t mode) {
    char* path = fs_fullpath(filename);

    if (!path) {
        log_error(logger, "Error al asignar memoria para el path.");
        return -1;
    }

    log_info(logger, "Ruta completa del archivo: [%s]", path);

    int fd = open(path, flags, mode);
    if (fd == -1) {
        log_error(logger, "Error al abrir el archivo: [%s]", path);
    }

    free(path);
    return fd;
}

DIR* fs_opendir (){
    // TODO: config.path_base_dialfs
    DIR* dir = opendir(config.path_base_dialfs);

    if (dir == NULL) {
        perror("No se puede abrir el directorio");
        return NULL;
    }

    return dir;
}

