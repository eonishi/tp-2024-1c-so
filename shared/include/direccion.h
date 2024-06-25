#ifndef DIRECCION_H_
#define DIRECCION_H_

#include <stdint.h>
#include <stdlib.h>
#include "protocolo.h"
#include "paquete.h"

typedef struct{
    uint32_t direccion_fisica;
    void *dato;
    size_t tam_dato;
} t_peticion_memoria;

t_peticion_memoria *peticion_crear(uint32_t direccion_fisica, void *dato, size_t tam_dato);
void peticion_destruir(t_peticion_memoria *direccion);
void peticion_enviar(t_peticion_memoria *peticion_a_enviar, op_code CODE, int socket);
t_peticion_memoria *peticion_recibir(int socket, op_code CODE);
void peticion_empaquetar(t_peticion_memoria *peticion, t_paquete *paquete, op_code CODE);
size_t peticiones_tam_total(t_list *peticiones);
void peticiones_distribuir_dato(t_list *peticiones, void *dato_entero, size_t tam_dato);
void peticion_escritura_enviar(void *peticion_escribir, int socket_memoria);

#endif