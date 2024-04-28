#include "../include/pcb.h"

pcb* crear_pcb(unsigned id, unsigned quantum){
    pcb* nuevo_pcb = (pcb*) malloc(sizeof(pcb));
    nuevo_pcb->pid = id;
    nuevo_pcb->pc = 0;
    nuevo_pcb->quantum = quantum;
    nuevo_pcb->registros->ax = 0;
    nuevo_pcb->registros->bx = 0;
    nuevo_pcb->registros->cx = 0;
    nuevo_pcb->registros->dx = 0;
    nuevo_pcb->registros->eax = 0;
    nuevo_pcb->registros->ebx = 0;
    nuevo_pcb->registros->ecx = 0;
    nuevo_pcb->registros->edx = 0;
    nuevo_pcb->registros->si = 0;
    nuevo_pcb->registros->di = 0;
    return nuevo_pcb;
}

int enviar_pcb(int socket_cliente, pcb* pcb){
    t_paquete* paquete = crear_paquete(DISPATCH_PROCESO);
    agregar_a_paquete(paquete, &pcb->pid, sizeof(unsigned));
    agregar_a_paquete(paquete, &pcb->pc, sizeof(unsigned));
    agregar_a_paquete(paquete, &pcb->quantum, sizeof(unsigned));
    agregar_a_paquete(paquete, &pcb->registros->ax, sizeof(uint8_t));
    agregar_a_paquete(paquete, &pcb->registros->bx, sizeof(uint8_t));
    agregar_a_paquete(paquete, &pcb->registros->cx, sizeof(uint8_t));
    agregar_a_paquete(paquete, &pcb->registros->dx, sizeof(uint8_t));
    agregar_a_paquete(paquete, &pcb->registros->eax, sizeof(uint32_t));
    agregar_a_paquete(paquete, &pcb->registros->ebx, sizeof(uint32_t));
    agregar_a_paquete(paquete, &pcb->registros->ecx, sizeof(uint32_t));
    agregar_a_paquete(paquete, &pcb->registros->edx, sizeof(uint32_t));
    agregar_a_paquete(paquete, &pcb->registros->si, sizeof(uint32_t));
    agregar_a_paquete(paquete, &pcb->registros->di, sizeof(uint32_t));
    enviar_paquete(paquete, socket_cliente);
    return 0;
}

pcb* recibir_pcb(int socket_cliente){
    t_list* lista = recibir_paquete(socket_cliente);
    list_iterate(lista, (void*)iterator);
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}