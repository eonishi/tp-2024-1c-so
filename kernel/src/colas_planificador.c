#include "../include/colas_planificador.h"

void inicializar_colas_planificador(){
    cola_new = queue_create();
	cola_ready = queue_create();
	cola_exit = queue_create();
}

void imprimir_colas(){
    log_error(logger, "Sin implementar!!");
}

void push_cola_new(pcb* pcb){
    queue_push(cola_new, pcb);
}

pcb* pop_cola_new(){
    queue_pop(cola_new);
}

void push_cola_ready(pcb* pcb){
    queue_push(cola_ready, pcb);
}

pcb* pop_cola_ready(){
    queue_pop(cola_ready);
}

void push_cola_exit(pcb* pcb){
    queue_push(cola_exit, pcb);
}



