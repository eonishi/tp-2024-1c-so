#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#define ERROR -1


#include "pcb.h"
#include "paquete.h"
#include "codigos_operacion.h"
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

// Temporales
void* serializar_char(char* string);
char* deserializar_char(void* char_bytes, int8_t size);

#endif