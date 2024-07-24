#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#define ERROR -1


#include<commons/log.h>
#include<commons/string.h>
#include<commons/collections/list.h>
#include<string.h>
#include "serializar.h"
#include "paquete.h"
#include "codigos_operacion.h"
#include "operacion.h"
#include "io_tipos.h"
#include "direccion.h"

extern t_log* logger;

// Kernel - Memory
typedef struct 
{
    unsigned PID;
    char* filePath;
} solicitud_crear_proceso;

int enviar_solicitud_crear_proceso(char* filePath, unsigned PID, int socket_cliente);
solicitud_crear_proceso recibir_solicitud_crear_proceso(int socket_cliente);

typedef struct{
    char *nombre_interfaz;
    io_tipo tipo;
    int* operaciones;
} solicitud_conexion_kernel;

void enviar_solicitud_conexion_kernel(solicitud_conexion_kernel, int socket_cliente);
solicitud_conexion_kernel recibir_solicitud_conexion_kernel(int socket_cliente);


typedef struct{
    int pid;
    char **tokens;
    t_list* peticiones_memoria;
} solicitud_instruccion_io;


int enviar_instruccion_io(char** instruccion_tokenizada, t_list* peticiones_memoria, int pid, int socket_cliente);
solicitud_instruccion_io recibir_instruccion_io(int socket_cliente);

typedef struct{
    char *nombre_archivo;
    int tamanio_archivo;
    int pid;
} solicitud_truncar_archivo;

void enviar_solicitud_truncar_archivo_fs(solicitud_truncar_archivo solicitud, int socket);
solicitud_truncar_archivo recibir_solicitud_truncar_archivo_fs(int socket);


typedef struct{
    char *nombre_archivo;
    int pid;
} solicitud_accion_archivo;

void enviar_solicitud_accion_archivo_fs(int accion, char* nombre, int pid, int socket);
solicitud_accion_archivo recibir_solicitud_accion_archivo_fs(int socket);
typedef struct{
    int retraso;
    int pid;
} solicitud_io_sleep;

void enviar_io_sleep(int retraso, int pid, int socket);
solicitud_io_sleep recibir_io_sleep(int socket);

#endif