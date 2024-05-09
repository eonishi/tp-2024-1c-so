#include "../include/fetch.h"

char* fetch(){
    pedir_intruccion_a_memoria(pcb_actual->pc);
    return recibir_instruccion_de_memoria(socket_memoria);
}

char *recibir_instruccion_de_memoria(int socket_memoria){
    int codigo_recibido = recibir_operacion(socket_memoria);
    if(codigo_recibido != FETCH_INSTRUCCION){
        log_error(logger, "El código de respuesta no es el esperado. Esperado: [%d]. Recibido: [%d]", FETCH_INSTRUCCION, codigo_recibido);
        return "FAIL";
    }

    int size;
    char *buffer = recibir_buffer(&size, socket_memoria);
    log_info(logger, "Mensaje recibido: [%s]", buffer);
    return buffer;
}

void pedir_intruccion_a_memoria(){ 
    t_paquete* paquete_solicitud = crear_paquete(FETCH_INSTRUCCION); // Contiene el PID y el PC

    void *pid_serializado = serializar_u(&(pcb_actual->pid), sizeof(pcb_actual->pid));
    void *pc_serializado = serializar_u(&(pcb_actual->pc), sizeof(pcb_actual->pc));

    agregar_a_paquete(paquete_solicitud, pid_serializado, sizeof(pcb_actual->pid)); 
    agregar_a_paquete(paquete_solicitud, pc_serializado, sizeof(pcb_actual->pc));
    enviar_paquete(paquete_solicitud, socket_memoria);
    log_info(logger, "Solicitud FETCH_INSTRUCCION_PID:%u_PC:%u enviada a memoria", pcb_actual->pid, pcb_actual->pc);

    // Como siempre no tengo idea todavia si hacer esto free son correctos 
    eliminar_paquete(paquete_solicitud);
    free(pid_serializado);
    free(pc_serializado);
}

void* serializar_u (void* datos, size_t size){
    void* datos_serializados = malloc(size);
    memcpy(datos_serializados, datos, size);
    return datos_serializados;
}