#include "../include/io_stdout.h"

static void controlar_peticion();

void io_stdout() {
     log_info(logger, "Hola, soy IO STDOUT :)");

     while (kernel_socket != -1 || memory_socket != -1){
          op_code operacion = recibir_operacion(kernel_socket);
          log_info(logger, "Operacion recibida de Kernel [%d]", operacion);

          switch (operacion){
               case EJECUTAR_INSTRUCCION_IO:
                    solicitud_instruccion_io solicitud = recibir_instruccion_io(kernel_socket);
                    int pid = solicitud.pid;
                    t_list *peticiones_memoria = solicitud.peticiones_memoria;
                    char** tokens_instr = solicitud.tokens;

                    log_info(logger,"RECIBIDO EJECUTAR_INSTRUCCION_IO, pid: [%d]", pid);

                    log_info(logger, "Instruccion recibida de Kernel [%s]", tokens_instr[0]);
                    log_peticiones(peticiones_memoria);

                    // [] Enviar peticiones a memoria y guardar el resultado
                    size_t tam_total = peticiones_tam_total(peticiones_memoria) + 1;
                    log_info(logger, "Tam total de las peticiones [%ld]", tam_total);
                    void* string_a_imprimir = malloc(tam_total);
                    memset(string_a_imprimir, 0, tam_total);
                    void* ptr_string = string_a_imprimir;

                    for (int i = 0; i < list_size(peticiones_memoria); i++){
                         t_peticion_memoria* peticion = list_get(peticiones_memoria, i);
                         log_info(logger, "Peticion [%d] de [%d]", i+1, list_size(peticiones_memoria));
                         peticion_lectura_enviar(peticion, &ptr_string, memory_socket);
                         controlar_peticion();
                    }
                    // [] Imprimir resultado          
                    printf("%s\n", (char*)string_a_imprimir);

                    // [] Enviar status a Kernel
                    enviar_status(FIN_EJECUCION_IO, kernel_socket);
                    break;

               case -1:
                    close(kernel_socket);
                    close(memory_socket);
                    exit(EXIT_FAILURE);
                    break;

               default:
                    log_error(logger, "Operacion no reconocida [%d]", operacion);
                    break;
          }
     }
     
}

static void controlar_peticion(){
     op_code status = recibir_operacion(memory_socket);
     if(status == SUCCESS){
        log_info(logger, "La operacion en memoria fue exitosa");
     }
     else{
        log_error(logger, "Hubo un problema con la operacion en memoria");
     }
}