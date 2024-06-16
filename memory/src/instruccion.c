#include "../include/instruccion.h"

static unsigned PC_solicitado;

static char* get_instr_by_pc(){
    if(!list_any_satisfy(procesos_en_memoria, memoria_tiene_pid_solicitado)){
        log_error(logger, "No se encontró el PID solicitado");
        return "PID NOT FOUND"; // Hacer un caso "FAIL" en decode o chequear antes de enviar a CPU
    }

    t_proceso_en_memoria* proceso_buscado = list_find(procesos_en_memoria, memoria_tiene_pid_solicitado);
    
    if(PC_solicitado >= list_size(proceso_buscado->instrucciones)){
        log_error(logger, "No hay más instrucciones del proceso PID:%u", PID_solicitado);
        return "OUT OF BOUNDS"; // Hacer un caso "FAIL" en decode o chequear antes de enviar a CPU
    }
    char* instruc_buscada = list_get (proceso_buscado->instrucciones, PC_solicitado);

    return instruc_buscada;
}

void enviar_instruccion_a_cpu(){
    char *instruccion = get_instr_by_pc();
    enviar_mensaje(FETCH_INSTRUCCION, instruccion, socket_cpu);
    log_info(logger, "Instruccion enviada a CPU: [%s]", instruccion);
}

void recibir_fetch_de_cpu(){
    t_list* paquete_recibido = recibir_paquete(socket_cpu);
    PID_solicitado = *(unsigned*)list_get(paquete_recibido, 0);
    PC_solicitado = *(unsigned*)list_get(paquete_recibido, 1);
    log_info(logger, "PID solicitado: %d", PID_solicitado);
    log_info(logger, "PC solicitado: %d", PC_solicitado);

    list_destroy(paquete_recibido);
}
