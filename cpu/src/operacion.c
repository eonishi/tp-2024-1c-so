#include "../include/operacion.h"

const table_element tabla_op[] = {
    {"SET", SET},
    {"MOV_IN", MOV_IN},
    {"MOV_OUT", MOV_OUT},
    {"SUM", SUM},
    {"SUB", SUB},
    {"JNZ", JNZ},
    {"RESIZE", RESIZE},
    {"COPY_STRING", COPY_STRING},
    {"WAIT", WAIT},
    {"SIGNAL", SIGNAL},
    {"IO_GEN_SLEEP", IO_GEN_SLEEP},
    {"IO_STDIN_READ", IO_STDIN_READ},
    {"IO_STDOUT_WRITE", IO_STDOUT_WRITE},
    {"IO_FS_CREATE", IO_FS_CREATE},
    {"IO_FS_DELETE", IO_FS_DELETE},
    {"IO_FS_TRUNCATE", IO_FS_TRUNCATE},
    {"IO_FS_WRITE", IO_FS_WRITE},
    {"IO_FS_READ", IO_FS_READ},
    {"EXIT", EXIT_OP}
}; // Lo hago para no tener que hacer la funcion con mas if anidados del mundo 

operacion get_operacion(char** inst_tokenizada) {
    int i;
    for (i = 0; i < sizeof(tabla_op) / sizeof(table_element); i++) {
        if (string_equals_ignore_case(tabla_op[i].key, inst_tokenizada[0])) {
            return tabla_op[i].value;
        }
    }
    // manejar el caso en que la operación no se reconoce. No va a haber peeeero 🤷🏻‍♂️
    return -1;
}

void* get_registro(char* token_registro){ // OOOHH NO TENGO QUE HACER LA FUNCION CON MÁS IF ANIDADOS DEL MUNDO :(
    if(string_equals_ignore_case(token_registro, "AX")) return &(pcb_actual->registros->ax);
    else if(string_equals_ignore_case(token_registro, "BX")) return &(pcb_actual->registros->bx);
    else if(string_equals_ignore_case(token_registro, "CX")) return &(pcb_actual->registros->cx);
    else if(string_equals_ignore_case(token_registro, "DX")) return &(pcb_actual->registros->dx);
    else if(string_equals_ignore_case(token_registro, "EAX")) return &(pcb_actual->registros->eax);
    else if(string_equals_ignore_case(token_registro, "EBX")) return &(pcb_actual->registros->ebx);
    else if(string_equals_ignore_case(token_registro, "ECX")) return &(pcb_actual->registros->ecx);
    else if(string_equals_ignore_case(token_registro, "EDX")) return &(pcb_actual->registros->edx);
    else if(string_equals_ignore_case(token_registro, "SI")) return &(pcb_actual->registros->si);
    else if(string_equals_ignore_case(token_registro, "DI")) return &(pcb_actual->registros->di);
    else if(string_equals_ignore_case(token_registro, "PC")) return &(pcb_actual->pc);
    else return NULL; // No va a haber casos de error de sintaxis pero qsy
}

int get_valor(char* token_valor){
    return atoi(token_valor);
}

bool es8int(char* token_registro){ // Voy lo mas rapido que puedo, NO LLEGO AL CHECKPOINT 2 🏃🏻
    return string_equals_ignore_case(token_registro, "AX") 
    || string_equals_ignore_case(token_registro, "BX") 
    || string_equals_ignore_case(token_registro, "CX") 
    || string_equals_ignore_case(token_registro, "DX");
}