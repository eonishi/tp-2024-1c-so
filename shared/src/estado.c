#include "../include/estado.h"

char* estadoToString(state estado){
    switch(estado){
        case NEW:
            return "NEW";
        case READY:
            return "READY";
        case READY_PRIORIDAD:
            return "READY_PRIORIDAD";
        case EXECUTE:
            return "EXEC";
        case BLOCKED:
            return "BLOCK";
        case EXIT:
            return "EXIT";
        default:
            return "UNKNOWN";
    }
}