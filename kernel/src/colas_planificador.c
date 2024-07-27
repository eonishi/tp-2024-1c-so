#include "../include/colas_planificador.h"
#define CANTIDAD_COLAS_ESTADO 5

t_queue *cola_new, *cola_exit, *cola_ready, *cola_execute, *cola_readyVRR;
pthread_mutex_t mutex_new, mutex_exit, mutex_ready, mutex_execute, mutex_readyVRR = PTHREAD_MUTEX_INITIALIZER;

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
    imprimir_cola("New", cola_new, mutex_new);
    imprimir_cola("Ready", cola_ready, mutex_ready);
    imprimir_cola("ReadyVRR", cola_readyVRR, mutex_readyVRR);
    imprimir_cola("Execute", cola_execute, mutex_execute);
    imprimir_cola("Exit", cola_exit, mutex_exit);
    imprimir_colas_io();
    imprimir_colas_recurso();
}

void imprimir_cola(char* nombre, t_queue* cola, pthread_mutex_t mutex){
    log_info(logger, "=====================================");
    log_info(logger, "Procesos en cola [%s]: ", nombre);

    pthread_mutex_lock(&mutex);
    for(int index = 0; index < cola->elements->elements_count; index++){
        pcb* pcb = list_get(cola->elements, index);

        log_info(logger, "--> PID: [%d] ESTADO: [%d]", pcb->pid, pcb->estado);
    }
    pthread_mutex_unlock(&mutex);
}

void imprimir_colas_io(){
    pthread_mutex_lock(&mutex_conexiones_io);
    for (size_t i = 0; i < list_size(lista_conexiones_io); i++){
        conexion_io* conexion_a_imprimir = list_get(lista_conexiones_io, i);
        imprimir_cola(conexion_a_imprimir->nombre_interfaz, conexion_a_imprimir->cola_espera, conexion_a_imprimir->mutex);
    }
    pthread_mutex_unlock(&mutex_conexiones_io);
}

void imprimir_colas_recurso(){
    pthread_mutex_lock(&mutex_recursos_disponibles);
    for (size_t i = 0; i < list_size(recursos_disponibles); i++)
    {
        t_recurso* recurso = list_get(recursos_disponibles, i);
        imprimir_cola(recurso->nombre, recurso->procesos_en_espera, recurso->mutex);
    }
    pthread_mutex_unlock(&mutex_recursos_disponibles);
}

void push_cola_new(pcb* pcb){
    log_info(logger, "Se crea el proceso <%d> en NEW", pcb->pid);

    pcb->estado = NEW;

    pthread_mutex_lock(&mutex_new);
        queue_push(cola_new, pcb);
    pthread_mutex_unlock(&mutex_new);

    sem_post(&sem_nuevo_proceso);
}

pcb* pop_cola_new(){
    pthread_mutex_lock(&mutex_new);
        pcb* pcb = queue_pop(cola_new);
    pthread_mutex_unlock(&mutex_new);
    return pcb;
}

void push_cola_ready(pcb* pcb){
    log_info(logger_oblig,"PID: <%d> - Estado Anterior: %s - Estado Actual: READY", pcb->pid, estadoToString(pcb->estado));
    pcb->estado = READY;

    pthread_mutex_lock(&mutex_ready);
        queue_push(cola_ready, pcb);
    pthread_mutex_unlock(&mutex_ready);

    sem_post(&sem_proceso_en_ready);
    imprimir_cola("READY:",cola_ready, mutex_ready);

}

pcb* pop_cola_ready(){
    pthread_mutex_lock(&mutex_ready);
        pcb* pcb = queue_pop(cola_ready);
    pthread_mutex_unlock(&mutex_ready);
    return pcb;
}

void push_cola_blocked(pcb* pcb, t_queue* cola_blocked, sem_t* sem_blocked, pthread_mutex_t mutex_blocked){

    log_info(logger_oblig,"PID: <%d> - Estado Anterior: %s - Estado Actual: BLOCKED", pcb->pid, estadoToString(pcb->estado));

    pcb->estado = BLOCKED;

    // pthread_mutex_lock(&mutex_blocked);
    queue_push(cola_blocked, pcb);
    // pthread_mutex_unlock(&mutex_blocked);

    sem_post(sem_blocked);
    log_info(logger, "sem_post sem_blocked");
}

pcb* pop_cola_blocked(t_queue* cola_blocked, pthread_mutex_t mutex_blocked){
    pthread_mutex_lock(&mutex_blocked);
    pcb* pcb = queue_pop(cola_blocked);
    pthread_mutex_unlock(&mutex_blocked);
    return pcb;
}

void push_cola_exit(pcb* pcb){
    log_info(logger_oblig,"PID: <%d> - Estado Anterior: %s - Estado Actual: EXIT", pcb->pid, estadoToString(pcb->estado));
    pcb->estado = EXIT;

    pthread_mutex_lock(&mutex_exit);
        queue_push(cola_exit, pcb);
    pthread_mutex_unlock(&mutex_exit);

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
    log_info(logger_oblig,"PID: %d - Estado Anterior: %s - Estado Actual: EXECUTE", pcb->pid, estadoToString(pcb->estado));
    pcb->estado = EXECUTE;
    pthread_mutex_lock(&mutex_execute);
        queue_push(cola_execute, pcb);
    pthread_mutex_unlock(&mutex_execute);
}

pcb* pop_cola_execute(){
    pthread_mutex_lock(&mutex_execute);
        pcb* pcb = queue_pop(cola_execute);
    pthread_mutex_unlock(&mutex_execute);
    return pcb;
}

void push_cola_ready_priority(pcb* pcb){
    log_info(logger_oblig,"PID: %d - Estado Anterior: %s - Estado Actual: READY PRIORIDAD", pcb->pid, estadoToString(pcb->estado));
    pcb->estado = READY;

    pthread_mutex_lock(&mutex_readyVRR);
        queue_push(cola_readyVRR, pcb);
    pthread_mutex_unlock(&mutex_readyVRR);
    imprimir_cola("READY PRIORIDAD:",cola_readyVRR, mutex_readyVRR);
}

pcb* pop_cola_ready_priority(){
    pcb* siguiente_pcb = pop_cola_blocked(cola_readyVRR, mutex_readyVRR);
    return siguiente_pcb;
}

void pop_and_destroy(t_queue* queue, void (*destroyer)(void*), pthread_mutex_t mutex){
    pthread_mutex_lock(&mutex);
        destroyer(queue_pop(queue));
    pthread_mutex_unlock(&mutex);
}

void pop_and_destroy_execute(){
    pop_and_destroy(cola_execute, (void (*)(void*))destruir_pcb, mutex_execute);
}

static pthread_mutex_t mutex_busqueda = PTHREAD_MUTEX_INITIALIZER;
static bool es_pid_buscado(void* pcb_buscado, unsigned PID_BUSQUEDA){
    pcb *pcb = pcb_buscado;
    return pcb->pid == PID_BUSQUEDA;
}

bool proceso_esta_en_cola(t_queue* queue, unsigned PID, pthread_mutex_t mutex){

    // Esto SOLO se puede hacer porque compilamos con GCC, es normal que el intellisense lo marque como error
    bool _wrap_es_pid(void* pcb_buscado){
        return es_pid_buscado(pcb_buscado, PID);
    }

    pthread_mutex_lock(&mutex);
        bool result = list_any_satisfy(queue->elements, _wrap_es_pid);
    pthread_mutex_unlock(&mutex);

    return result;
}

pcb* pop_proceso(t_queue* queue, unsigned PID, pthread_mutex_t mutex){

    // Esto SOLO se puede hacer porque compilamos con GCC, es normal que el intellisense lo marque como error
    bool _wrap_es_pid(void* pcb_buscado){
        return es_pid_buscado(pcb_buscado, PID);
    }

    pthread_mutex_lock(&mutex);
        pcb* pcb = list_remove_by_condition(queue->elements, _wrap_es_pid);
    pthread_mutex_unlock(&mutex);

    return pcb;
}

bool sacar_proceso_de_cola_estado(unsigned PID){
    // 😭😭 voy a tener que ir uno por uno (ya no me copa el mutex)
  
    if(proceso_esta_en_cola(cola_execute, PID, mutex_execute)){
        interrumpir_proceso_ejecutando(INTERRUPCION_USUARIO);
        return true;
    }

    if(proceso_esta_en_cola(cola_new, PID, mutex_new)){
        pcb* pcb_encontrado = pop_proceso(cola_new, PID, mutex_new);
        push_cola_exit(pcb_encontrado);
        return true;
    }

    if(proceso_esta_en_cola(cola_ready, PID, mutex_ready)){
        pcb* pcb_encontrado = pop_proceso(cola_ready, PID, mutex_ready);
        push_cola_exit(pcb_encontrado);
        return true;
    }

    //if(proceso_esta_en_cola(cola_readyVRR, PID, mutex_readyVRR)){
    //    elemVRR* elem = pop_proceso(cola_readyVRR, PID, mutex_readyVRR);
    //    push_cola_exit(elem->pcbVRR);
    //    return true;
    //}

    if(proceso_esta_en_cola(cola_exit, PID, mutex_exit)){
        log_warning(logger, "Proceso [%d] ya se encuentra en EXIT", PID);
        return true;
    }

    return false;
}

bool sacar_proceso_de_cola_io(unsigned PID){
    for (size_t i = 0; i < list_size(lista_conexiones_io); i++){
        conexion_io* conexion = list_get(lista_conexiones_io, i);
        if(proceso_esta_en_cola(conexion->cola_espera, PID, conexion->mutex)){
            pcb* pcb_encontrado = pop_proceso(conexion->cola_espera, PID, conexion->mutex);
            push_cola_exit(pcb_encontrado);
            return true;
        }
    }
    return false;
}

bool sacar_proceso_de_cola_recurso(unsigned PID){
    for (size_t i = 0; i < list_size(recursos_disponibles); i++){
        t_recurso* recurso = list_get(recursos_disponibles, i);
        if(proceso_esta_en_cola(recurso->procesos_en_espera, PID, recurso->mutex)){
            pcb* pcb_encontrado = pop_proceso(recurso->procesos_en_espera, PID, recurso->mutex);
            push_cola_exit(pcb_encontrado);
            return true;
        }
    }
    return false;
}

typedef bool (*sacar_proceso_func)(unsigned PID);

void finalizar_proceso(unsigned PID){

    sacar_proceso_func funciones[] = {
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

