#ifndef IO_FS_UTILS_H_
#define IO_FS_UTILS_H_

#include <commons/config.h>
#include <commons/log.h>
#include <stdlib.h>
#include <string.h>
#include "configuracion.h"
#include "dirent.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>


extern t_log *logger;
extern io_config config;

int fs_open(const char* path, int flags, mode_t mode);
DIR* fs_opendir ();

char* concat_chars(char* str1, char* str2);

#endif