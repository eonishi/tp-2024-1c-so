#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#define ERROR -1


#include "pcb.h"
#include "paquete.h"
#include "codigos_operacion.h"
#include "io_tipos.h"
#include<commons/log.h>
#include<string.h>

extern t_log* logger;

// Kernel - Memory
typedef struct 
{
    pcb* pcb;
    char* filePath;
} solicitud_crear_proceso;

int enviar_solicitud_crear_proceso(char* filePath, pcb* pcb, int socket_cliente);
solicitud_crear_proceso recibir_solicitud_crear_proceso(int socket_cliente);

typedef struct{
    char *nombre_interfaz;
    io_tipo tipo;
} solicitud_conexion_kernel;

void enviar_solicitud_conexion_kernel(solicitud_conexion_kernel, int socket_cliente);
solicitud_conexion_kernel recibir_solicitud_conexion_kernel(int socket_cliente);

// Temporales
void* serializar_char(char* string);
char* deserializar_char(void* char_bytes, int8_t size);

#endif