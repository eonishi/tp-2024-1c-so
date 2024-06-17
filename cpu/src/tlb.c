#include "../include/tlb.h"

t_list *tlb;

void iniciar_tlb(){
    tlb = list_create();
}

bool tlb_tiene_espacio(){
    return list_size(tlb) < config.cantidad_entradas_tlb;
}

static unsigned pagina_solicitada;
static unsigned PID_solicitada;

bool es_entrada_buscada(void *entrada){
    tlb_entry *entrada_buscada = (tlb_entry*)entrada;
    bool resultado = (
        entrada_buscada->PID == PID_solicitada &&
        entrada_buscada->pagina == pagina_solicitada
    );
    return resultado;
}

bool tlb_tiene_entrada(unsigned pid, unsigned pagina){
    pagina_solicitada = pagina;
    PID_solicitada = pid;
    return list_any_satisfy(tlb, es_entrada_buscada);
}

unsigned obtener_frame_tlb(unsigned pid, unsigned pagina){
    pagina_solicitada = pagina;
    PID_solicitada = pid;
    tlb_entry *entrada = list_find(tlb, es_entrada_buscada);
    entrada->uso_tiempo = 0;
    return entrada->frame;
}

void liberar_entrada_tlb(tlb_entry *entrada){
    free(entrada);
}

void tlb_reemplazar_por_fifo(tlb_entry* nueva_entrada){
    tlb_entry *entrada = list_remove(tlb, 0);
    liberar_entrada_tlb(entrada);
    list_add(tlb, &nueva_entrada);
}

tlb_entry *tlb_entry_mayor_uso_tiempo(void *entrada1, void *entrada2){
    tlb_entry* entrada_mayor = (tlb_entry*) entrada1;
    tlb_entry* entrada_menor = (tlb_entry*) entrada2;
    return entrada_mayor->uso_tiempo > entrada_menor->uso_tiempo ? entrada_mayor : entrada_menor;
}

void tlb_reemplazar_por_lru(tlb_entry* nueva_entrada){
    tlb_entry *entrada_a_eliminar = list_fold1(tlb, (void*)tlb_entry_mayor_uso_tiempo);

    if(list_remove_element(tlb, entrada_a_eliminar)) 
        liberar_entrada_tlb(entrada_a_eliminar);
    else 
        log_error(logger, "No se pudo eliminar la entrada PID:[%d], Pagina:[%d], Frame:[%d], Uso tiempo:[%d]", 
            entrada_a_eliminar->PID, 
            entrada_a_eliminar->pagina, 
            entrada_a_eliminar->frame, 
            entrada_a_eliminar->uso_tiempo);    

    list_add(tlb, &nueva_entrada);
}

void iterate_incrementar_uso_tiempo(void *entrada){
       tlb_entry* entrada_a_incrementar = (tlb_entry*) entrada;
        entrada_a_incrementar->uso_tiempo++;
}

void incrementar_uso_tiempo_tlb(){
    list_iterate(tlb, iterate_incrementar_uso_tiempo);
}

void agregar_entrada_tlb(unsigned pid, unsigned pagina, unsigned frame){
    tlb_entry nueva_entrada = {
        .PID = pid,
        .pagina = pagina,
        .frame = frame,
        .uso_tiempo = 0
    };
    if(tlb_tiene_espacio()){
        list_add(tlb, &nueva_entrada);
    }
    else{
        if(string_equals_ignore_case(config.algoritmo_tlb, "FIFO")){
            tlb_reemplazar_por_fifo(&nueva_entrada);
        }
        else{
            tlb_reemplazar_por_lru(&nueva_entrada);
        }
    }

    incrementar_uso_tiempo_tlb();
}
