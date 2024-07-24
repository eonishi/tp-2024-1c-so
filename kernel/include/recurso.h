#ifndef RECURSO_H_
#define RECURSO_H_

#include "configuracion.h"
#include <semaphore.h>
#include <pthread.h>
#include <commons/string.h>
#include <commons/collections/queue.h>
#include "colas_planificador.h"
#include "../../shared/include/pcb.h"
#include "../../shared/include/hilo.h"

typedef struct {
    char *nombre;
    sem_t instancias;
    sem_t procesos_en_cola;
    t_queue *procesos_en_espera;
    t_list *PIDs;
} t_recurso;

extern t_queue *cola_execute;
extern kernel_config *config;

void inicializar_recursos();
t_recurso *get_recurso(char *nombre_recurso);
bool recurso_disponible(t_recurso *nombre_recurso);
void wait_recurso(t_recurso *recurso, unsigned PID);
void signal_recurso(t_recurso* recurso, unsigned PID);
void esperar_recurso(t_recurso* recurso, pcb *pcb);
bool recurso_existe(char *nombre_recurso);
void liberar_recurso_del_proceso(unsigned PID);

#endif