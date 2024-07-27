#include "../include/codigos_operacion.h"

const char* traduce_cod_op(op_code code) {
    switch (code) {
        case MENSAJE: return "MENSAJE";
        case PAQUETE: return "PAQUETE";
        case HANDSHAKE: return "HANDSHAKE";
        case CREAR_PROCESO_EN_MEMORIA: return "CREAR_PROCESO_EN_MEMORIA";
        case LIBERAR_PROCESO_EN_MEMORIA: return "LIBERAR_PROCESO_EN_MEMORIA";
        case DISPATCH_PROCESO: return "DISPATCH_PROCESO";
        case FETCH_INSTRUCCION: return "FETCH_INSTRUCCION";
        case REDIMENSIONAR_MEMORIA_PROCESO: return "REDIMENSIONAR_MEMORIA_PROCESO";
        case ESCRIBIR_DATO_EN_MEMORIA: return "ESCRIBIR_DATO_EN_MEMORIA";
        case LEER_DATO_DE_MEMORIA: return "LEER_DATO_DE_MEMORIA";
        case DATO_LEIDO_DE_MEMORIA: return "DATO_LEIDO_DE_MEMORIA";
        case CONSULTAR_TABLA_DE_PAGINAS: return "CONSULTAR_TABLA_DE_PAGINAS";
        case RESPUESTA: return "RESPUESTA";
        case INTERRUPCION_USUARIO: return "INTERRUPCION_USUARIO";
        case INTERRUPCION_QUANTUM: return "INTERRUPCION_QUANTUM";
        case PROCESO_TERMINADO: return "PROCESO_TERMINADO";
        case PROCESO_BLOQUEADO_IO: return "PROCESO_BLOQUEADO";
        case PROCESO_SOLICITA_RECURSO: return "PROCESO_SOLICITA_RECURSO";
        case PROCESO_LIBERA_RECURSO: return "PROCESO_LIBERA_RECURSO";
        case PROCESO_DESALOJADO: return "PROCESO_DESALOJADO";
        case ERROR_DE_PROCESAMIENTO: return "ERROR_DE_PROCESAMIENTO";
        case EJECUTAR_INSTRUCCION_IO: return "EJECUTAR_INSTRUCCION_IO";
        case FIN_EJECUCION_IO: return "FIN_EJECUCION_IO";
        case SUCCESS: return "SUCCESS";
        case OUT_OF_MEMORY: return "OUT_OF_MEMORY";
        default: return "UNKNOWN";
    }
}