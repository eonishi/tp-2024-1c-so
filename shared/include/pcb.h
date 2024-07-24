#ifndef PCB_H_
#define PCB_H_

#include <stdint.h>
#include "comunicacion.h"
#include "paquete.h"
#include "codigos_operacion.h"
#include "solicitud_io.h"
#include "serializar.h"
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

typedef enum
{
    NEW='N',
    READY='R',
    EXECUTE='E',
    BLOCKED='B',
    EXIT='X'
} state;

typedef struct
{
    unsigned pid;
    unsigned pc; 
    unsigned quantum;
    registros_t* registros;
    state estado; //N=NEW, E=EXECUTE, X=EXIT, B=BLOCKED, R=READY
    solicitud_bloqueo_por_io* solicitud;
} pcb;

pcb* crear_pcb(unsigned id, unsigned quantum);
registros_t* crear_registros();
void enviar_pcb(pcb* pcb, int socket_cliente, op_code code);
pcb *recibir_pcb(int socket_cliente);
pcb* deserializar_pcb_new(void* pcb_bytes);
int pcb_size();
void* serializar_pcb(pcb* pcb);
void *serializar_pcb_data_primitive(pcb *pcb);
void* serializar_registros(registros_t* registros);
pcb* deserializar_pcb(void* pcb_data_primitive, void* pcb_data_registers);
registros_t *deserializar_registros(void *registros);
void loggear_pcb(pcb *pcb);
void destruir_pcb(pcb* pcb);
void siguiente_pc(pcb* pcb);

#endif