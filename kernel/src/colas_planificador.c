#include "../include/colas_planificador.h"
#define CANTIDAD_COLAS_ESTADO 5

t_queue *cola_new;
t_queue *cola_exit;
t_queue *cola_ready;
t_queue *cola_execute;
t_queue *cola_readyVRR;

t_queue* colas_estado[CANTIDAD_COLAS_ESTADO]; 

void inicializar_colas_planificador(){
    cola_new = queue_create();
	cola_ready = queue_create();
	cola_exit = queue_create();
    cola_execute = queue_create();
    cola_readyVRR = queue_create();

    colas_estado[0] = cola_new;
    colas_estado[1] = cola_ready;
    colas_estado[2] = cola_readyVRR;
    colas_estado[3] = cola_execute;
    colas_estado[4] = cola_exit;

    log_info(logger, "Colas de planificacion inicializadas");
}

void imprimir_colas(){
    imprimir_cola("New", cola_new);
    imprimir_cola("Ready", cola_ready);
    imprimir_cola("ReadyVRR", cola_readyVRR);
    imprimir_cola("Execute", cola_execute);
    imprimir_cola("Exit", cola_exit);
    imprimir_colas_io();
    imprimir_colas_recurso();
}

void imprimir_cola(char* nombre, t_queue* cola){
    log_info(logger, "=====================================");
    log_info(logger, "Procesos en cola [%s]: ", nombre);
    for(int index = 0; index < cola->elements->elements_count; index++){
        pcb* pcb = list_get(cola->elements, index);

        log_info(logger, "--> PID: [%d] ESTADO: [%d]", pcb->pid, pcb->estado);
    }
}

void imprimir_colas_io(){
    for (size_t i = 0; i < list_size(lista_conexiones_io); i++){
        conexion_io* conexion_a_imprmir = list_get(lista_conexiones_io, i);
        imprimir_cola(conexion_a_imprmir->nombre_interfaz, conexion_a_imprmir->cola_espera);
    }
}

void imprimir_colas_recurso(){
    for (size_t i = 0; i < list_size(recursos_disponibles); i++){
        t_recurso* recurso = list_get(recursos_disponibles, i);
        log_info(logger, "=====================================");
        log_info(logger, "Procesos en cola de recurso [%s]: ", recurso->nombre);
        for(int index = 0; index < list_size(recurso->procesos_en_espera->elements); index++){
            pcb* pcb = list_get(recurso->procesos_en_espera->elements, index);
            log_info(logger, "--> PID: [%d] ESTADO: [%d]", pcb->pid, pcb->estado);
        }
    }

}

void push_cola_new(pcb* pcb){
    log_info(logger, "Se crea el proceso <%d> en NEW", pcb->pid);

    pcb->estado = NEW;
    queue_push(cola_new, pcb);
    sem_post(&sem_nuevo_proceso);
}

pcb* pop_cola_new(){
    return queue_pop(cola_new);
}

void push_cola_ready(pcb* pcb){
    log_info(logger_oblig,"PID: %d - Estado Anterior: %c - Estado Actual: READY", pcb->pid, pcb->estado);
    pcb->estado = READY;
    queue_push(cola_ready, pcb);
    sem_post(&sem_proceso_en_ready);
    imprimir_cola("READY:",cola_ready);

}

pcb* pop_cola_ready(){
    return queue_pop(cola_ready);
}

void push_cola_blocked(pcb* pcb, t_queue* cola_blocked, sem_t* sem_blocked){

    log_info(logger_oblig,"PID: %d - Estado Anterior: %c - Estado Actual: BLOCKED", pcb->pid, pcb->estado);
    pcb->estado = BLOCKED;
    queue_push(cola_blocked, pcb);
    sem_post(sem_blocked);
}

void push_cola_exit(pcb* pcb){
    log_info(logger_oblig,"PID: %d - Estado Anterior: %c - Estado Actual: EXIT", pcb->pid, pcb->estado);
    pcb->estado = EXIT;
    queue_push(cola_exit, pcb);

    // Liberar recursos
    liberar_recurso_del_proceso(pcb->pid);

    if(diferencia_grado_multiprog < 0)
        diferencia_grado_multiprog++;
    else
        sem_post(&sem_grado_multiprog);

    // Envio operacion de liberar el proceso en memoria
    enviar_cantidad(pcb->pid, LIBERAR_PROCESO_EN_MEMORIA, socket_memoria);
    op_code status = recibir_operacion(socket_memoria);
    log_info(logger, "Proceso [%d] liberado con estado [%d]", pcb->pid, status);
}

void push_cola_execute(pcb* pcb){
    log_info(logger_oblig,"PID: %d - Estado Anterior: %c - Estado Actual: EXECUTE", pcb->pid, pcb->estado);
    pcb->estado = EXECUTE;
    queue_push(cola_execute, pcb);
}

pcb* pop_cola_execute(){
    return queue_pop(cola_execute);
}

void push_cola_ready_priority(pcb* pcb, int quantum_pendiente){
    log_info(logger_oblig,"PID: %d - Estado Anterior: %c - Estado Actual: READY PRIORIDAD", pcb->pid, pcb->estado);
    pcb->estado = READY;
    elemVRR element = {.pcbVRR = pcb, .quantum_usado = quantum_pendiente};
    queue_push(cola_readyVRR, &element);
    imprimir_cola("READY PRIORIDAD:",cola_readyVRR);
}

elemVRR* pop_cola_ready_priority(){
    return queue_pop(cola_readyVRR);
}

void pop_and_destroy(t_queue* queue, void (*destroyer)(void*)){
    destroyer(queue_pop(queue));
}

static unsigned PID_BUSQUEDA;
static bool es_pid_buscado(void* pcb_buscado){
    pcb *pcb = pcb_buscado;
    return pcb->pid == PID_BUSQUEDA;
}

bool proceso_esta_en_cola(t_queue* queue, unsigned PID){
    PID_BUSQUEDA = PID;
    return list_any_satisfy(queue->elements, es_pid_buscado);
}

pcb* pop_proceso(t_queue* queue, unsigned PID){
    PID_BUSQUEDA = PID;
    return list_remove_by_condition(queue->elements, es_pid_buscado);
}

bool sacar_proceso_de_cola_estado(unsigned PID){
    for (size_t i = 0; i < CANTIDAD_COLAS_ESTADO; i++){
        if(proceso_esta_en_cola(colas_estado[i], PID)){
            pcb* pcb_encontrado = pop_proceso(colas_estado[i], PID);
            push_cola_exit(pcb_encontrado);
            return true;
        }
    }
    return false;
}

bool sacar_proceso_de_cola_io(unsigned PID){
    for (size_t i = 0; i < list_size(lista_conexiones_io); i++){
        conexion_io* conexion = list_get(lista_conexiones_io, i);
        if(proceso_esta_en_cola(conexion->cola_espera, PID)){
            pcb* pcb_encontrado = pop_proceso(conexion->cola_espera, PID);
            push_cola_exit(pcb_encontrado);
            return true;
        }
    }
    return false;
}

bool sacar_proceso_de_cola_execute(unsigned PID){
    if(proceso_esta_en_cola(cola_execute, PID)){
        interrumpir_proceso_ejecutando(INTERRUPCION_USUARIO);
        return true;
    }
    return false;
}

bool sacar_proceso_de_cola_recurso(unsigned PID){
    for (size_t i = 0; i < list_size(recursos_disponibles); i++){
        t_recurso* recurso = list_get(recursos_disponibles, i);
        if(proceso_esta_en_cola(recurso->procesos_en_espera, PID)){
            pcb* pcb_encontrado = pop_proceso(recurso->procesos_en_espera, PID);
            push_cola_exit(pcb_encontrado);
            return true;
        }
    }
    return false;
}

typedef bool (*sacar_proceso_func)(unsigned PID);

void finalizar_proceso(unsigned PID){
    PID_BUSQUEDA = PID;

    sacar_proceso_func funciones[] = {
        sacar_proceso_de_cola_execute,
        sacar_proceso_de_cola_estado,
        sacar_proceso_de_cola_io,
        sacar_proceso_de_cola_recurso
    };

    for (size_t i = 0;  i < sizeof(funciones) / sizeof(funciones[0]); i++) {
        if (funciones[i](PID)) {
            liberar_recurso_del_proceso(PID);
            return;
        }
    }

    log_warning(logger, "Proceso [%d] no encontrado, intente nuevamente", PID);    

    // Si el PID se encuentra en una cola de estado/IO/recurso, se lo saca y se lo manda a EXIT
    // sino no hace nada
    //sacar_proceso_de_cola_execute(PID);
    //sacar_proceso_de_cola_estado(PID);
    //sacar_proceso_de_cola_io(PID);
    // [] sacar_proceso_de_cola_io(PID);
    //liberar_recurso_del_proceso(PID);
    /* DISCLAIMER:
     *  Si se quisiera finalizar el proceso en ejecucion, hay condicion de carrera
     *  entre el checkeo de la condicion entre todas las colas y la "devolucion" del
     *  proceso en ejecucion.
     *  TODO: Implementar semaforo para evitar la condicion de carrera
     */
}

