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
void peticion_escritura_enviar(t_peticion_memoria *peticion_escribir, int socket_memoria);
void peticion_lectura_enviar(t_peticion_memoria *peticion_a_enviar, void **ptr_donde_se_guarda_dato, int socket_memoria);
t_peticion_memoria *peticion_recibir(int socket, op_code CODE);

void peticion_empaquetar(t_peticion_memoria *peticion, t_paquete *paquete, op_code CODE);
void peticiones_empaquetar(t_list *peticiones, t_paquete *paquete);
t_list *peticiones_desempaquetar(t_list *paquete_lista);
t_list *peticiones_desempaquetar_segun_index(t_list *paquete_lista, int lista_index);

void *serializar_peticion_memoria(t_peticion_memoria *peticion);
void *deserializar_peticion_memoria(void *stream);

void log_peticion(void *peticion_bytes);
void log_peticiones(t_list *peticiones);

void peticiones_distribuir_dato(t_list *peticiones, void *dato_entero, size_t tam_dato);
size_t peticiones_tam_total(t_list *peticiones);

#endif