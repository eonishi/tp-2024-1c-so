#include "../include/ciclo.h"

pcb* pcb_actual;
int tengo_pcb = 0;

void cicloDeCPU(){
    while(tengo_pcb){
        log_info(logger, "Fetching...");
        char* instruccion = fetch();
        log_info(logger, "Instrucción obtenida: [%s]...", instruccion);
        if(strcmp(instruccion, "FAIL") == 0){
            log_error(logger, "Fallo al obtener instrucción. Instrucción: [%s]...", instruccion);
            tengo_pcb = 0;
            break;
        }
        char** instruccion_tokenizada = decode(instruccion);
        log_info(logger, "Instrucción tokenizada correctamente");
        execute(instruccion_tokenizada);
        log_info(logger, "Instrucción ejecutada correctamente");
        //poner marca de tiempo fin paso execute
        checkInterrupt();
        log_info(logger, "======================================");
    }
}