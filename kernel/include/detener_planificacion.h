#ifndef DETENER_PLANIFICACION_H_
#define DETENER_PLANIFICACION_H_

#include <semaphore.h>
#include <pthread.h>
#include <stdbool.h>

void detener_planificacion();
void reanudar_planificacion();
void esperar_planificacion();

#endif