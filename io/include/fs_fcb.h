#ifndef IO_FS_FCB_H_
#define IO_FS_FCB_H_

#include <commons/config.h>
#include <string.h>


extern t_log *logger;

typedef struct 
{
    char* nombre;
    t_config* config;
} fcb;

// Utils fcb_list
void inicializar_lista_fcb();
int cantidad_de_fcbs();
void crear_fcb(fcb* fcb);
fcb* obtener_fcb(int index);
fcb* obtener_fcb_por_nombre(char* nombre);
void eliminar_fcb_por_nombre(char* nombre);
bool condicion_por_nombre(void* file_control_block);
void set_campo_de_archivo(char* campo , int valor, t_config* config_loader);

#endif