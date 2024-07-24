#ifndef SOLICITUD_IO_H_
#define SOLICITUD_IO_H_

#include <commons/string.h>
#include "protocolo.h"
#include "direccion.h"
#include "paquete.h"

typedef struct 
{
    char** instruc_io_tokenizadas;
    t_list* peticiones_memoria;
} solicitud_bloqueo_por_io;

int enviar_bloqueo_por_io(solicitud_bloqueo_por_io solicitud, int socket_cliente);
solicitud_bloqueo_por_io recibir_solicitud_bloqueo_por_io(int socket_cliente);
solicitud_bloqueo_por_io* crear_solicitud_io(char** instruc_io_tokenizadas, t_list* peticiones_memoria);
void liberar_solicitud_io(solicitud_bloqueo_por_io *solicitud);
void agregar_a_paquete_solicitud_io(solicitud_bloqueo_por_io *solicitud, t_paquete *paquete);

#endif