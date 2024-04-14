#include "../include/logger.h"

t_log* iniciar_logger(char* nombre_archivo, char* nombre_proceso)
{
	t_log* nuevo_logger;

	nuevo_logger = log_create(nombre_archivo, nombre_proceso, 1, LOG_LEVEL_INFO);

	return nuevo_logger;
}
