#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#define ERROR -1


#include "pcb.h"
#include "paquete.h"
#include "codigos_operacion.h"
#include "operacion.h"
#include "io_tipos.h"
#include "direccion.h"
#include<commons/log.h>
#include<commons/string.h>
#include<string.h>

extern t_log* logger;

// Kernel - Memory
typedef struct 
{
    unsigned PID;
    char* filePath;
} solicitud_crear_proceso;

int enviar_solicitud_crear_proceso(char* filePath, unsigned PID, int socket_cliente);
solicitud_crear_proceso recibir_solicitud_crear_proceso(int socket_cliente);

typedef struct 
{
    char** instruc_io_tokenizadas;
    pcb* pcb;    
    t_list* peticiones_memoria;
} solicitud_bloqueo_por_io;

typedef struct{
    char *nombre_interfaz;
    io_tipo tipo;
    int* operaciones;
} solicitud_conexion_kernel;

void enviar_solicitud_conexion_kernel(solicitud_conexion_kernel, int socket_cliente);
solicitud_conexion_kernel recibir_solicitud_conexion_kernel(int socket_cliente);

int enviar_bloqueo_por_io(solicitud_bloqueo_por_io solicitud, int socket_cliente);
solicitud_bloqueo_por_io recibir_solicitud_bloqueo_por_io(int socket_cliente);

int enviar_instruccion_io(char** instruccion_tokenizada, t_list* peticiones_memoria, int socket_cliente);
char** recibir_instruccion_io(int socket_cliente, t_list** ptr_peticiones_memoria);

int enviar_solicitud_leer_dato_de_memoria(uint32_t direccion, int socket_cliente);
uint32_t recibir_solicitud_leer_dato_de_memoria(int socket_cliente);

void enviar_dato_leido_de_memoria(uint32_t dato, int socket);
uint32_t recibir_dato_leido_de_memoria(int socket);


typedef struct{
    char *nombre_archivo;
    int tamanio_archivo;
} solicitud_truncar_archivo;

void enviar_solicitud_truncar_archivo_fs(solicitud_truncar_archivo solicitud, int socket);
solicitud_truncar_archivo recibir_solicitud_truncar_archivo_fs(int socket);


// Temporales
void* serializar_char(char* string);
void* serializar_int(int number);
void* serializar_uint32(uint32_t value);
void* serializar_lista_strings(char** strings);

char** deserializar_lista_strings(t_list* bytes, int index_cantidad_tokens);
int deserializar_int(void *int_bytes);
char* deserializar_char(void* char_bytes, int8_t size);
uint32_t deserializar_uint32(void* int_bytes);

#endif