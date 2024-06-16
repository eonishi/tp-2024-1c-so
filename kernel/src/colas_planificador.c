#include "../include/colas_planificador.h"

void inicializar_colas_planificador(){
    cola_new = queue_create();
	cola_ready = queue_create();
	cola_blocked = queue_create();
	cola_exit = queue_create();
    cola_execute = queue_create();
}

void imprimir_colas(){
    imprimir_cola("New", cola_new);
    imprimir_cola("Ready", cola_ready);
    imprimir_cola("Blocked", cola_blocked);
    imprimir_cola("Exit", cola_exit);
}

void imprimir_cola(char* nombre, t_queue* cola){
    log_info(logger, "=====================================");
    log_info(logger, "Procesos en cola [%s]: ", nombre);
    for(int index = 0; index < cola->elements->elements_count; index++){
        pcb* pcb = list_get(cola->elements, index);

        log_info(logger, "--> PID: [%d] ESTADO: [%d]", pcb->pid, pcb->estado);
    }
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

void push_cola_blocked(pcb* pcb){
    queue_push(cola_blocked, pcb);
}

pcb* pop_cola_blocked(){
    queue_pop(cola_blocked);
}

void push_cola_exit(pcb* pcb){
    queue_push(cola_exit, pcb);
}

void push_cola_execute(pcb* pcb){
    queue_push(cola_execute, pcb);
}

pcb* pop_cola_execute(){
    queue_pop(cola_execute);
}


