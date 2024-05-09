#ifndef PCB_H_
#define PCB_H_

#include "protocolo.h"
#include "comunicacion.h"
#include "paquete.h"
#include <stdint.h>

typedef struct 
{
    uint8_t ax;
    uint8_t bx;
    uint8_t cx;
    uint8_t dx;
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t si; // Source Index de un string
    uint32_t di; // Destination Index de un string
} registros_t;

typedef struct 
{
    unsigned pid;
    unsigned pc; 
    unsigned quantum;
    registros_t* registros;
    char estado; //N=NEW, E=EXECUTE, X=EXIT, B=BLOCK
    //state estado; // enum {NEW, READY, EXEC, BLOCKED, EXIT}
    //list_t ios //lista de IOs que usa (es util y vale la pena para el contexto del proyecto? el libro lo recomienda)
}pcb; 

pcb* crear_pcb(unsigned id, unsigned quantum);
registros_t* crear_registros();
int enviar_pcb(pcb* pcb, int socket_cliente, op_code code);
pcb *recibir_pcb(int socket_cliente);
void *serializar_pcb_data_primitive(pcb *pcb);
void* serializar_registros(registros_t* registros);
pcb* deserializar_pcb(void* pcb_data_primitive, void* pcb_data_registers);
registros_t *deserializar_registros(void *registros);
void loggear_pcb(pcb *pcb);
pcb* esperar_pcb(int socket, op_code codigo_esperado);
void destruir_pcb(pcb* pcb);
void siguiente_pc(pcb* pcb);
#endif