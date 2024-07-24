#ifndef HILO_H_
#define HILO_H_

#include <pthread.h>
#include <commons/log.h>
#include <string.h>

extern t_log* logger;

void iniciar_hilo(void* func);
void iniciar_hilo_con_args(void *(*func)(void *), void* args);

#endif