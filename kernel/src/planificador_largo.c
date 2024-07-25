#include "../include/planificador_largo.h"

void *iniciar_planificacion_largo(){
    while(1){

        sem_wait(&sem_grado_multiprog); // 2
        log_info(logger, "Largo: Esperando nuevo proceso....");
        sem_wait(&sem_nuevo_proceso); // 2
        log_info(logger, "Largo: Ingresó nuevo proceso");

        esperar_planificacion();
        
        pcb *newPcb = pop_cola_new();
        push_cola_ready(newPcb);
        
    }
}