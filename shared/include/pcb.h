#ifndef PCB_H_
#define PCB_H_

#include <stdint.h>

typedef struct 
{
    uint32_t pc; // Program Counter
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
    registros_t registros_ejecucion;
    //state estado; // enum {NEW, READY, EXEC, BLOCKED, EXIT}
    //list_t ios //lista de IOs que usa (es util y vale la pena para el contexto del proyecto? el libro lo recomienda)
}pcb; 

#endif