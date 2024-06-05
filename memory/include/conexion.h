#ifndef CONEXION_MEMORIA_H_
#define CONEXION_MEMORIA_H_

#include <commons/log.h>
#include "configuracion.h"
#include "../../shared/include/server.h"
#include "../../shared/include/client.h"
#include "../../shared/include/comunicacion.h"

extern t_log* logger;
extern memory_config config;

void esperar_handshake_kernel(int server);
void esperar_handshake_cpu(int server);

#endif