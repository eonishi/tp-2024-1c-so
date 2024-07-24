#include "../include/recurso.h"

t_list* recursos_disponibles;

static void* esperar_recurso_thread_handler(void* args){
    // recibir el recurso por arg;
    t_recurso* recurso = (t_recurso*)args;

    while (1){
        sem_wait(&recurso->procesos_en_cola);
        pcb *proceso_espera = queue_peek(recurso->procesos_en_espera);
        log_warning(logger, "Proceso [%d] en espera de la cola [%s]", proceso_espera->pid, recurso->nombre);

        wait_recurso(recurso, proceso_espera->pid);
        proceso_espera = queue_pop(recurso->procesos_en_espera);
        push_cola_ready(proceso_espera);
    }
}

void inicializar_recursos(){
    recursos_disponibles = list_create();

    for (size_t i = 0; config->recursos[i] != NULL ; i++){
        t_recurso* nuevo_recurso = malloc(sizeof(t_recurso));
        int instancias_recurso = atoi(config->instancias_recursos[i]);

        nuevo_recurso->nombre = config->recursos[i]; // Aca tengo duda porque el recurso y la config apuntan al mismo nombre
        sem_init(&(nuevo_recurso->instancias), 0, instancias_recurso);
        sem_init(&(nuevo_recurso->procesos_en_cola), 0, 0);
        nuevo_recurso->procesos_en_espera = queue_create();
        nuevo_recurso->PIDs = list_create(); 

        list_add(recursos_disponibles, nuevo_recurso);

        iniciar_hilo_con_args(esperar_recurso_thread_handler, nuevo_recurso);
    }
    
    log_info(logger, "Recursos inicializados");
}

t_recurso* get_recurso(char* nombre_recurso){
    for (size_t i = 0; i < list_size(recursos_disponibles); i++){
        t_recurso* recurso = list_get(recursos_disponibles, i);
        if (string_equals_ignore_case(recurso->nombre, nombre_recurso)){
            return recurso;
        }
    }
    return NULL;
}

bool recurso_existe(char* nombre_recurso){
    log_warning(logger, "Estoy buscando el recurso: %s", nombre_recurso);
    for (size_t i = 0; i < list_size(recursos_disponibles); i++){
        t_recurso* recurso = list_get(recursos_disponibles, i);
        log_warning(logger, "Recurso: %s", recurso->nombre);
        if (string_equals_ignore_case(recurso->nombre, nombre_recurso)){
            return true;
        }
    }
    return false;
}

bool recurso_disponible(t_recurso* recurso){
    int valor_semaforo;
    sem_getvalue(&(recurso->instancias), &valor_semaforo);
    log_warning(logger, "Valor semaforo: %d", valor_semaforo);
    return valor_semaforo > 0;
}

void wait_recurso(t_recurso* recurso, unsigned PID){
    sem_wait(&(recurso->instancias));

    unsigned* PID_aux = malloc(sizeof(unsigned));
    *PID_aux = PID;
    list_add(recurso->PIDs, PID_aux);
}

static unsigned PID_buscado;
static bool es_PID(void* PID){
    return *(unsigned*)PID == PID_buscado;
}

void signal_recurso(t_recurso* recurso, unsigned PID){
    sem_post(&(recurso->instancias));

    PID_buscado = PID;
    if(list_any_satisfy(recurso->PIDs, es_PID)){
        list_remove_and_destroy_by_condition(recurso->PIDs, es_PID, free);
    }
}

void esperar_recurso(t_recurso* recurso, pcb* pcb){
    pop_and_destroy(cola_execute, (void*) destruir_pcb);
    push_cola_blocked(pcb, recurso->procesos_en_espera, &recurso->procesos_en_cola);
}

void liberar_recurso_del_proceso(unsigned PID){
    PID_buscado = PID;
    for (size_t i = 0; i < list_size(recursos_disponibles); i++)
    {
        t_recurso* recurso = list_get(recursos_disponibles, i);
        if(list_any_satisfy(recurso->PIDs, es_PID)){
            signal_recurso(recurso, PID);
        }
    }
}