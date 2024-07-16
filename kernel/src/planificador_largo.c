#include "../include/planificador_largo.h"

void *iniciar_planificacion_largo(){
    while(1){
        if(planificacion_activada){               
            sem_wait(&sem_grado_multiprog);  // 2
            log_info(logger, "Largo: Esperando nuevo proceso....");
            sem_wait(&sem_nuevo_proceso);  // 2
            log_info(logger, "Largo: Ingresó nuevo proceso");
            

            pcb* newPcb = pop_cola_new();
            //newPcb->estado = READY; //el cambio de estado se maneja dentro de las funciones push

            push_cola_ready(newPcb);

            sem_post(&sem_proceso_en_ready);
        }
    }
}