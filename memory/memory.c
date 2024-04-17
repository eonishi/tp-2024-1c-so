#include "include/memory.h"

int main(){
    t_log* logger = iniciar_logger("memory.log", "MEMORY");
    log_info(logger, "Logger de MEMORY iniciado");

    log_info(logger, "Iniciando servidor");
    int server_fd = iniciar_servidor("MEMORY", "127.0.0.1", "8888", logger);
    int client_fd = esperar_cliente(server_fd);

    return 0;
}

/**

Esta parte de la memoria será la encargada de obtener de los archivos de pseudo código las instrucciones y 
de devolverlas a pedido a la CPU.
Al momento de recibir la creación de un proceso, la memoria de instrucciones deberá leer el archivo de pseudocódigo indicado y
generar las estructuras que el grupo considere necesarias para poder devolver las instrucciones de a 1 a la CPU 
según ésta se las solicite por medio del Program Counter.
Ante cada petición se deberá esperar un tiempo determinado a modo de retardo en la obtención de la instrucción, y este tiempo,
 estará indicado en el archivo de configuración.


**/