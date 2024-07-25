#include "../include/detener_planificacion.h"

bool planificacion_activada = true;
pthread_mutex_t mutex_planificacion_activa = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_planificacion_activa;

void detener_planificacion() {
    pthread_mutex_lock(&mutex_planificacion_activa);
        planificacion_activada = false;
    pthread_mutex_unlock(&mutex_planificacion_activa);
}

void reanudar_planificacion() {
    // Si la planificación ya está activada, no hago nada
    if(planificacion_activada) return;

    planificacion_activada = true;
    sem_post(&sem_planificacion_activa);
}

void esperar_planificacion() {
    pthread_mutex_lock(&mutex_planificacion_activa);
        while (!planificacion_activada) {
            sem_wait(&sem_planificacion_activa);
        }
    pthread_mutex_unlock(&mutex_planificacion_activa);
}