#ifndef ESTADO_H_
#define ESTADO_H_

typedef enum
{
    NEW,
    READY,
    READY_PRIORIDAD,
    EXECUTE,
    BLOCKED,
    EXIT
} state;

char *estadoToString(state estado);

#endif