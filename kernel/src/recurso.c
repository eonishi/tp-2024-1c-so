#include "../include/recurso.h"

t_list* recursos_disponibles;
pthread_mutex_t mutex_recursos_disponibles = PTHREAD_MUTEX_INITIALIZER;
bool wait_recurso_de_espera(t_recurso *recurso, unsigned PID_buscado);

static void *esperar_recurso_thread_handler(void *args)
{
    // recibir el recurso por arg;
    t_recurso* recurso = (t_recurso*)args;

    while (1){

        sem_wait(&recurso->procesos_en_cola);
        esperar_planificacion();
        
        pcb *proceso_espera = queue_peek(recurso->procesos_en_espera);
        log_warning(logger, "Proceso [%d] en espera de la cola [%s]", proceso_espera->pid, recurso->nombre);
        if(!wait_recurso_de_espera(recurso, proceso_espera->pid)){
            continue;
        }
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
        pthread_mutex_init(&(nuevo_recurso->mutex), NULL);

        list_add(recursos_disponibles, nuevo_recurso);

        iniciar_hilo_con_args(esperar_recurso_thread_handler, nuevo_recurso);
    }
    
    log_info(logger, "Recursos inicializados");
}

t_recurso* get_recurso(char* nombre_recurso){
    t_recurso* recurso_encontrado = NULL;

    pthread_mutex_lock(&mutex_recursos_disponibles);
        for (size_t i = 0; i < list_size(recursos_disponibles); i++){
            t_recurso* recurso = list_get(recursos_disponibles, i);
            if (string_equals_ignore_case(recurso->nombre, nombre_recurso)){
                recurso_encontrado = recurso;
            }
        }
    pthread_mutex_unlock(&mutex_recursos_disponibles);
    return recurso_encontrado;
}

bool recurso_existe(char* nombre_recurso){
    log_warning(logger, "Estoy buscando el recurso: %s", nombre_recurso);

    bool recurso_encontrado = false;

    pthread_mutex_lock(&mutex_recursos_disponibles);
        for (size_t i = 0; i < list_size(recursos_disponibles); i++){
            t_recurso* recurso = list_get(recursos_disponibles, i);
            log_warning(logger, "Recurso: %s", recurso->nombre);
            if (string_equals_ignore_case(recurso->nombre, nombre_recurso)){
                recurso_encontrado = true;
            }
        }
    pthread_mutex_unlock(&mutex_recursos_disponibles);


    return recurso_encontrado;
}

bool recurso_disponible(t_recurso* recurso){
    int valor_semaforo;
    sem_getvalue(&(recurso->instancias), &valor_semaforo);
    log_warning(logger, "Valor semaforo: %d", valor_semaforo);
    return valor_semaforo > 0;
}

static bool es_PID(void* PID, unsigned PID_buscado){
    return *(unsigned*)PID == PID_buscado;
}

void wait_recurso(t_recurso* recurso, unsigned PID_buscado){
    sem_wait(&(recurso->instancias));

    unsigned* PID_aux = malloc(sizeof(unsigned));
    *PID_aux = PID_buscado;
    list_add(recurso->PIDs, PID_aux);
}

bool wait_recurso_de_espera(t_recurso* recurso, unsigned PID_buscado){
    sem_wait(&(recurso->instancias));

    bool _wrap_es_pid(void* PID){
        return es_PID(PID, PID_buscado);
    }

    if(!list_any_satisfy(recurso->procesos_en_espera->elements, _wrap_es_pid)){
        sem_post(&(recurso->instancias));
        return false;
    }

    unsigned* PID_aux = malloc(sizeof(unsigned));
    *PID_aux = PID_buscado;
    list_add(recurso->PIDs, PID_aux);
    return true;
}

void signal_recurso(t_recurso* recurso, unsigned PID_buscado){
    sem_post(&(recurso->instancias));

    // Esto SOLO se puede hacer porque compilamos con GCC, es normal que el intellisense lo marque como error
    bool _wrap_es_pid(void* PID){
        return es_PID(PID, PID_buscado);
    }

    if(list_any_satisfy(recurso->PIDs, _wrap_es_pid)){
        list_remove_and_destroy_all_by_condition(recurso->PIDs, _wrap_es_pid, free);
    }
}

void esperar_recurso(t_recurso* recurso, pcb* pcb){
    pop_and_destroy_execute();
    push_cola_blocked(pcb, recurso->procesos_en_espera, &recurso->procesos_en_cola, recurso->mutex);
}

void liberar_recurso_del_proceso(unsigned PID_buscado){
    // Esto SOLO se puede hacer porque compilamos con GCC, es normal que el intellisense lo marque como error
    bool _wrap_es_pid(void* PID){
        return es_PID(PID, PID_buscado);
    }

    pthread_mutex_lock(&mutex_recursos_disponibles);
        for (size_t i = 0; i < list_size(recursos_disponibles); i++) {
            t_recurso* recurso = list_get(recursos_disponibles, i);
            if(list_any_satisfy(recurso->PIDs, _wrap_es_pid)){
                signal_recurso(recurso, PID_buscado);
            }
        }
    pthread_mutex_unlock(&mutex_recursos_disponibles);
}