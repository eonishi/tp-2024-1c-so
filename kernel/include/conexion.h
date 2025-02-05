#ifndef CONEXION_KERNEL_H_
#define CONEXION_KERNEL_H_

#include <semaphore.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <stdlib.h>
#include "configuracion.h"
#include "../../shared/include/client.h"
#include "../../shared/include/comunicacion.h"
#include "../../shared/include/server.h"
#include "../../shared/include/protocolo.h"
#include "../../shared/include/io_tipos.h"

extern t_log *logger;
extern t_log *logger_oblig;
extern kernel_config* config;
extern int socket_cpu_dispatch, socket_cpu_interrupt, socket_memoria, socket_io, socket_server_kernel;

typedef struct{
    int socket;
    char *nombre_interfaz;
    io_tipo tipo;
    int* operaciones;
    t_queue* cola_espera;
    sem_t sem_cliente;
    pthread_mutex_t mutex;
} conexion_io;

extern t_list* lista_conexiones_io;


conexion_io* crear_conexion_io(int socket, char* nombre_interfaz, io_tipo tipo, int* operaciones);
void liberar_conexion_io(conexion_io *conexion_io);

bool generar_conexiones();
int generar_conexion(char* ip, char* puerto);

conexion_io* recibir_conexion_io(int server);
void *esperar_y_escuchar_conexiones_io();

#endif