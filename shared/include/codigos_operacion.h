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
	RESPUESTA,
	INTERRUPCION,
	PROCESO_TERMINADO,
	PROCESO_BLOQUEADO,
	PROCESO_DESALOJADO,

	ERROR_DE_PROCESAMIENTO,
	// IO
	EJECUTAR_INTRUCCION_IO,
	// respuestas
	SUCCESS
} op_code;

#define ERROR -1

#endif