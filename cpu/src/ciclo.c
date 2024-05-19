#include "../include/ciclo.h"

pcb* pcb_actual;
int tengo_pcb = 0;

void cicloDeCPU(){
    while(tengo_pcb){
        char* instruccion = fetch();
        char** instruccion_tokenizada = decode(instruccion);
        execute(instruccion_tokenizada);
        checkInterrupt();
    }
}