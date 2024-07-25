#include "../include/consola.h"

static int pcb_counter = 1; // Variable global para llevar la referencia de los PID
int diferencia_grado_multiprog = 0;

const table_element tabla_comandos[] = {
	{"INICIAR_PROCESO", INICIAR_PROCESO},
	{"FINALIZAR_PROCESO", FINALIZAR_PROCESO},
	{"INICIAR_PLANIFICACION", INICIAR_PLANIFICACION},
	{"DETENER_PLANIFICACION", DETENER_PLANIFICACION},
	{"MULTIPROGRAMACION", MULTIPROGRAMACION},
	{"PROCESO_ESTADO", PROCESO_ESTADO},
	{"EJECUTAR_SCRIPT", EJECUTAR_SCRIPT},
	{"INICIAR_PLANIFICADOR", INICIAR_PLANIFICADOR}
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

        //case INICIAR_PLANIFICADOR:
        //    crear_hilo_planificador_corto();//permite crear un nuevo hilo de planificador luego de detenerlo
		//	//agregar un flag de control para no crear dos hilos de estos
        //    break;

        case INICIAR_PLANIFICACION:
            planificacion_activada = 1;
			//log_info(logger,"planificacion valor: %d",planificacion_activada);
            break;

        case DETENER_PLANIFICACION:
            planificacion_activada = 0;
			interrumpir_proceso_ejecutando();
			//cancelar_hilo_planificador(); //DETIENE EL HILO DEL PLANIFICADOR CORTO
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
			log_info(logger, "==============================================");
			char *string_pid = linea_leida[1];
			unsigned pid_a_finalizar = atol(string_pid);
			log_info(logger, "Finaliza el proceso <%d> - Motivo: INTERRUPTED_BY_USER", pid_a_finalizar); //validar log minimo
			//log_info(logger, "Inicio de ejecución de FINALIZAR_PROCESO con PID: [%d]", pid_a_finalizar);
			// Detener la planificación si está activada
			bool reanudar_planificacion = 0;
			if(planificacion_activada){
				log_info(logger, "Deteniendo la planificación para buscar el proceso a finalizar");

				char *string_detener_planificacion = "DETENER_PLANIFICACION";
				gestionar_comando_leido(&string_detener_planificacion);

				reanudar_planificacion = 1;
			}

			log_info(logger, "Buscando el proceso a finalizar");
			finalizar_proceso(pid_a_finalizar);

			// Reanudar la planificación si se finalizó un proceso con la planificación activada
			if(reanudar_planificacion){
				log_info(logger, "Reanudando la planificación");
				char *string_iniciar_planificacion = "INICIAR_PLANIFICACION";
				gestionar_comando_leido(&string_iniciar_planificacion);
			}

			break;

		case MULTIPROGRAMACION:
			int valor = get_valor(linea_leida[1]);
			log_info(logger, "Cambio de grado de multiprogacion de: [%d] a: [%d]", config->grado_multiprogramacion ,valor);

			diferencia_grado_multiprog = valor - config->grado_multiprogramacion;

			if(diferencia_grado_multiprog > 0){
				for(int i = 0; i < diferencia_grado_multiprog; i++){
					sem_post(&sem_grado_multiprog);
				}
			}
			
			config->grado_multiprogramacion = valor;
			break;

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
		log_info(aux_log, "Linea ingresada: %s", leido);

		char** leido_split = string_n_split(leido, 2, " ");
		char* comando_token = leido_split[0];

		if (string_equals_ignore_case(comando_token, "")) 
			ingresoActivado = 0;

		gestionar_comando_leido(leido_split);
		free(leido_split);
		free(leido);
	}
}
/*
void crear_hilo_planificador_corto(){
	log_info(logger, "algortimo: %s", config->algoritmo_planificacion);
	if(string_equals_ignore_case(config->algoritmo_planificacion, "FIFO")){
		log_info(logger, "Creando hilo para el planificador de corto plazo FIFO...");
		iniciar_hilo(iniciar_planificacion_corto, hilo_planificador_corto);
	}else if(string_equals_ignore_case(config->algoritmo_planificacion, "RR")){
		log_info(logger, "Creando hilo para el planificador de corto plazo ROUND ROBIN...");
		iniciar_hilo(iniciar_planificacion_corto_RR, hilo_planificador_corto_RR);
	}else if(string_equals_ignore_case(config->algoritmo_planificacion, "VRR")){
		log_info(logger, "Creando hilo para el planificador de corto plazo VIRTUAL ROUND ROBIN...");
		iniciar_hilo(iniciar_planificacion_corto_VRR, hilo_planificador_corto_VRR);
	}else{
		log_error(logger, "Algoritmo de planificación desconocido.");
		terminar_programa();
	}
}

void cancelar_hilo_planificador(){
	if (string_equals_ignore_case(config->algoritmo_planificacion,"FIFO")){
		pthread_cancel(hilo_planificador_corto);
		log_info(logger,"HILO PLANIFICADOR FIFO CANCELADO");
	}else if(string_equals_ignore_case(config->algoritmo_planificacion,"RR")){
		pthread_cancel(hilo_planificador_corto_RR);
		log_info(logger,"HILO PLANIFICADOR RR CANCELADO");
	}else if(string_equals_ignore_case(config->algoritmo_planificacion,"VRR")){
		pthread_cancel(hilo_planificador_corto_VRR);
		log_info(logger,"HILO PLANIFICADOR VRR CANCELADO");
	}
}
*/