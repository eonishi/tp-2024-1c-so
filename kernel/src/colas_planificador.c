#include "../include/colas_planificador.h"

void inicializar_colas_planificador(){
    cola_new = queue_create();
	cola_ready = queue_create();
	cola_blocked = queue_create();
	cola_exit = queue_create();
    cola_execute = queue_create();
    cola_readyVRR = queue_create();
}

void imprimir_colas(){
    imprimir_cola("New", cola_new);
    imprimir_cola("Ready", cola_ready);
    imprimir_cola("Blocked", cola_blocked);
    imprimir_cola("Execute", cola_execute);
    imprimir_cola("Exit", cola_exit);
    imprimir_cola("ReadyVRR", cola_readyVRR);
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
    pcb->estado = NEW;
    queue_push(cola_new, pcb);
}

pcb* pop_cola_new(){
    return queue_pop(cola_new);
}

void push_cola_ready(pcb* pcb){
    pcb->estado = READY;
    queue_push(cola_ready, pcb);
}

pcb* pop_cola_ready(){
    return queue_pop(cola_ready);
}

void push_cola_blocked(pcb* pcb){
    pcb->estado = BLOCKED;
    queue_push(cola_blocked, pcb);
}

pcb* pop_cola_blocked(){
    return queue_pop(cola_blocked);
}

void push_cola_exit(pcb* pcb){
    pcb->estado = EXIT;
    queue_push(cola_exit, pcb);

    // Envio operacion de liberar el proceso en memoria
    enviar_cantidad(pcb->pid, LIBERAR_PROCESO_EN_MEMORIA, socket_memoria);
    op_code status = recibir_operacion(socket_memoria);
    log_info(logger, "Proceso [%d] liberado con estado [%d]", pcb->pid, status);
}

void push_cola_execute(pcb* pcb){
    pcb->estado = EXECUTE;
    queue_push(cola_execute, pcb);
}

pcb* pop_cola_execute(){
    return queue_pop(cola_execute);
}

void push_cola_ready_priority(pcb* pcb, int quantum_pendiente){
    pcb->estado = READY;
    elemVRR element = {.pcbVRR = pcb, .quantum_usado = quantum_pendiente};
    queue_push(cola_readyVRR, &element);
}

elemVRR* pop_cola_ready_priority(){
    return queue_pop(cola_readyVRR);
}

void pop_and_destroy(t_queue* queue, void (*destroyer)(void*)){
    destroyer(queue_pop(queue));
}

