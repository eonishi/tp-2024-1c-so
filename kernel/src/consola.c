#include "../include/consola.h"

static int pcb_counter = 1; // Variable global para llevar la referencia de los PID

const table_element tabla_comandos[] = {
	{"INICIAR_PROCESO", INICIAR_PROCESO},
	{"FINALIZAR_PROCESO", FINALIZAR_PROCESO},
	{"INICIAR_PLANIFICACION", INICIAR_PLANIFICACION},
	{"DETENER_PLANIFICACION", DETENER_PLANIFICACION},
	{"MULTIPROGRAMACION", MULTIPROGRAMACION},
	{"PROCESO_ESTADO", PROCESO_ESTADO},
	{"EJECUTAR_SCRIPT", EJECUTAR_SCRIPT}
};

static comando_consola get_comando(char* comando_token){
	unsigned table_size = sizeof(tabla_comandos) / sizeof(table_element);
	return get_table_value(comando_token, tabla_comandos, table_size);
}

static void gestionar_comando_leido(char** linea_leida){
	char* comando_token = linea_leida[0];
	comando_consola comando = get_comando(comando_token);
		
    switch (comando) {
        case PROCESO_ESTADO:
            imprimir_colas();
            break;

        case INICIAR_PLANIFICACION:
            planificacion_activada = 1;
            break;

        case DETENER_PLANIFICACION:
            planificacion_activada = 0;
            break;

        case INICIAR_PROCESO:
            log_info(logger, "==============================================");
			log_info(logger, "Inicio de ejecución de INICIAR_PROCESO");

			char* filePath = linea_leida[1];

			log_info(logger, "Con argumento: [%s]", filePath);

            pcb *nuevo_pcb = crear_pcb(pcb_counter++, config->quantum);
            enviar_solicitud_crear_proceso(filePath, nuevo_pcb->pid, socket_memoria);

            if(recibir_operacion(socket_memoria) != SUCCESS){
                log_error(logger, "Error al crear el proceso en memoria");
                destruir_pcb(nuevo_pcb);
                break;
            }

			push_cola_new(nuevo_pcb);
			sem_post(&sem_nuevo_proceso);

			log_info(logger, "==============================================");
            break;

		case EJECUTAR_SCRIPT:
			char *path = linea_leida[1];
			log_info(logger, "Leer archivo de script: [%s]", path);

			FILE *archivo = fopen(path, "r");
			if (archivo == NULL){
				log_error(logger, "Error al abrir el archivo de instrucciones");
				exit(EXIT_FAILURE); // A checkear el exit 😅
			}

			char *linea = NULL; // NULL es importante para que getline haga el malloc automaticamente
			size_t len = 0;     // len = 0 es importante para que getline haga el malloc automaticamente
			ssize_t read;
			while ((read = getline(&linea, &len, archivo)) != -1) {
				char *mi_linea = string_new();
				string_n_append(&mi_linea, linea, string_length(linea));

				// Remueve todos los caracteres vacios de la derecha y la izquierda
				string_trim(&mi_linea);
				if(string_length(mi_linea) < 4) continue;
				char **instruccion_tokenizada = string_array_new();
    			instruccion_tokenizada = string_split(mi_linea, " ");
				gestionar_comando_leido(instruccion_tokenizada);
			}

			fclose(archivo);
			free(path);
			free(linea);
			break;
		case FINALIZAR_PROCESO:
		case MULTIPROGRAMACION:

        default:
            log_warning(logger, "Comando desconocido");
            break;
        }
}

void iniciar_consola()
{
	char* leido;
	leido = "void";
	bool ingresoActivado = 1;

	while(ingresoActivado){
		log_info(logger, "Ingrese INICIAR_PROCESO:");

		leido = readline("> ");
		log_info(logger, "Linea ingresada: %s", leido);

		char** leido_split = string_n_split(leido, 2, " ");
		char* comando_token = leido_split[0];

		if (string_equals_ignore_case(comando_token, "")) 
			ingresoActivado = 0;

		gestionar_comando_leido(leido_split);
		free(leido);
	}
}