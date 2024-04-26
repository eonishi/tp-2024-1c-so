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