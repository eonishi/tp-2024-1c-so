#ifndef ESTADO_H_
#define ESTADO_H_

typedef enum
{
    NEW,
    READY,
    EXECUTE,
    BLOCKED,
    EXIT
} state;

char *estadoToString(state estado);

#endif