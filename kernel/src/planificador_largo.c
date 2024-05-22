#include "../include/planificador_largo.h"

void inicializar_cola_new(){
    colaNew = queue_create();
}

void insertar_en_cola_new(pcb* pcb){
    queue_push(colaNew, pcb);
}

void *iniciar_planificacion_largo(){
    while(1){
        if(planificacion_activada){        
            sem_wait(&sem_grado_multiprog);  
            sem_wait(&sem_nuevo_proceso);  

            pcb* newPcb = queue_pop(colaNew);
            newPcb->estado = READY;

            queue_push(colaReady, newPcb);

            sem_post(&sem_proceso_en_ready);
        }
    }
}