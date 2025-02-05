#include "../include/registro.h"

void* get_registro(char* token_registro){ 
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
    else return NULL; 
}

uint32_t reg_to_uint32(void* valor, char* token){
    if(string_equals_ignore_case(token, "AX") || string_equals_ignore_case(token, "BX") || string_equals_ignore_case(token, "CX") || string_equals_ignore_case(token, "DX") ) 
        return *(uint8_t*) valor;
    
    return *(uint32_t*) valor;
}

bool es8int(char* token_registro){
    return string_equals_ignore_case(token_registro, "AX") 
    || string_equals_ignore_case(token_registro, "BX") 
    || string_equals_ignore_case(token_registro, "CX") 
    || string_equals_ignore_case(token_registro, "DX");
}

unsigned tam_registro(char* token_registro){
    return es8int(token_registro) ? sizeof(uint8_t) : sizeof(uint32_t);
}