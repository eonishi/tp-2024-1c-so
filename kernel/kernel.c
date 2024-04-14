#include "include/kernel.h"

int main(){
    t_log* logger = iniciar_logger("kernel.log", "KERNEL");

    log_info(logger, "Prueba de log");

    int a = 3; int b=14;

    printf("%d\n", sumar(a,b));

    return 0;
}

/*
El módulo Kernel, en el contexto de nuestro trabajo práctico, será el encargado de gestionar la ejecución de los 
diferentes procesos que se generen por medio de su consola interactiva.

*/