#include "../include/consola.h"

static int pcb_counter = 1; // Variable global para llevar la referencia de los PID

const table_element tabla_comandos[] = {
	{"INICIAR_PROCESO", INICIAR_PROCESO},
	{"FINALIZAR_PROCESO", FINALIZAR_PROCESO},
	{"INICIAR_PLANIFICACION", INICIAR_PLANIFICACION},
	{"DETENER_PLANIFICACION", DETENER_PLANIFICACION},
	{"MULTIPROGRAMACION", MULTIPROGRAMACION},
	{"PROCESO_ESTADO", PROCESO_ESTADO}
};

static comando_consola get_comando(char* comando_token){
	unsigned table_size = sizeof(tabla_comandos) / sizeof(table_element);
	return get_table_value(comando_token, tabla_comandos, table_size);
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

        comando_consola comando = get_comando(comando_token);
		
        switch (comando)
        {
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

			char* filePath = leido_split[1];

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

        default:
            log_warning(logger, "Comando desconocido");
            break;
        }

		free(leido);
	}
}