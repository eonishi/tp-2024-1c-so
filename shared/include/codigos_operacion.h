#ifndef OP_CODES_H_
#define OP_CODES_H_

typedef enum
{
	MENSAJE,
	PAQUETE,
	HANDSHAKE,
	INICIAR_PROCESO,
	CREAR_PROCESO_EN_MEMORIA,
	DISPATCH_PROCESO,
	FETCH_INSTRUCCION,
	REDIMENSIONAR_MEMORIA_PROCESO,
	ESCRIBIR_DATO_EN_MEMORIA,
	LEER_DATO_DE_MEMORIA,
	DATO_LEIDO_DE_MEMORIA,
	RESPUESTA,
	INTERRUPCION,
	PROCESO_TERMINADO,
	PROCESO_BLOQUEADO,
	PROCESO_DESALOJADO,
	ERROR_DE_PROCESAMIENTO,
	EJECUTAR_INSTRUCCION_IO,
	FIN_EJECUCION_IO,
	// IO
	// respuestas
	SUCCESS,
	OUT_OF_MEMORY
} op_code;

#define ERROR -1

#endif