#include "../include/planificador_largo.h"

void inicializar_cola_new(){
    cola_new = queue_create();
}

void inicializar_cola_exit(){
    cola_exit = queue_create();
}

void insertar_en_cola_new(pcb* pcb){
    queue_push(cola_new, pcb);
}

void *iniciar_planificacion_largo(){
    while(1){
        if(planificacion_activada){       
            sem_wait(&sem_grado_multiprog);  // 2
            sem_wait(&sem_nuevo_proceso);  // 2

            pcb* newPcb = queue_pop(cola_new);
            newPcb->estado = READY;

            queue_push(cola_ready, newPcb);

            sem_post(&sem_proceso_en_ready);
        }
    }
}