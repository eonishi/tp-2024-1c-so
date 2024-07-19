#include "../include/pcb.h"

// Tamaño de los registros y PCB
const int size_registros = sizeof(uint8_t) * 4 + sizeof(uint32_t) * 6;
const int size_primitive_data = sizeof(unsigned) * 3 + sizeof(state);
const int size_pcb = size_primitive_data + size_registros;
int pcb_size(){
    return size_pcb;
}

pcb* crear_pcb(unsigned id, unsigned quantum){
    pcb* nuevo_pcb = (pcb*) malloc(sizeof(pcb));
    nuevo_pcb->registros = (registros_t*) malloc(sizeof(registros_t));

    nuevo_pcb->pid = id;
    nuevo_pcb->quantum = quantum;
    nuevo_pcb->pc = 0;
    nuevo_pcb->registros = crear_registros();
    nuevo_pcb->estado = NEW;
    nuevo_pcb->solicitud_io = NULL;
    return nuevo_pcb;
}

registros_t* crear_registros(){
    registros_t* registros = (registros_t*) malloc(sizeof(registros_t));
    registros->ax = 0;
    registros->bx = 0;
    registros->cx = 0;
    registros->dx = 0;
    registros->eax = 0;
    registros->ebx = 0;
    registros->ecx = 0;
    registros->edx = 0;
    registros->si = 0;
    registros->di = 0;
    return registros;
}

void enviar_pcb(pcb* pcb, int socket_cliente, op_code code){
    t_paquete* paquete = crear_paquete(code);

    void* data_primitive = serializar_pcb_data_primitive(pcb); // Datos con tipos de datos primitivos de C dentro del PCB
    void* registros = serializar_registros(pcb->registros);
    
    agregar_a_paquete(paquete, data_primitive, size_primitive_data);
    agregar_a_paquete(paquete, registros, size_registros);

    if(pcb->solicitud_io == NULL){
        log_warning(logger, "Enviando PCB sin soliciutd");
        void* envio_una_solicitud = serializar_int(false);
        agregar_a_paquete(paquete, envio_una_solicitud, sizeof(int));
    }
    else {
    // Tengo que enviar instrucciones?
        bool envio_instrucciones = pcb->solicitud_io->instruc_io_tokenizadas != NULL;
        void* stream_envio_instrucciones = serializar_int(envio_instrucciones);
        agregar_a_paquete(paquete, stream_envio_instrucciones, sizeof(bool));

        log_warning(logger, "Enviando PCB con instrucciones: [%s]", envio_instrucciones? "true" : "false");

        if(envio_instrucciones){
            serializar_lista_strings_y_agregar_a_paquete(pcb->solicitud_io->instruc_io_tokenizadas, paquete);
        }

        // Tengo que enviar peticiones?
        bool envio_peticiones = pcb->solicitud_io->peticiones_memoria != NULL;
        void* stream_envio_peticiones = serializar_int(envio_peticiones);
        agregar_a_paquete(paquete, stream_envio_peticiones, sizeof(bool));

        log_warning(logger, "Enviando PCB con peticiones: [%s]", envio_peticiones? "true" : "false");
        if(envio_peticiones){
            peticiones_empaquetar(pcb->solicitud_io->peticiones_memoria, paquete);
        }

    }

    enviar_paquete(paquete, socket_cliente);

    free(data_primitive);
    free(registros);
    free(paquete->buffer);
    free(paquete);
}

pcb* recibir_pcb(int socket_cliente){
    t_list* lista_pcb_bytes = recibir_paquete(socket_cliente);

    void* pcb_bytes_primitive = list_get(lista_pcb_bytes, 0);
    void* pcb_bytes_registers = list_get(lista_pcb_bytes, 1);

    pcb* recived_pcb = deserializar_pcb(pcb_bytes_primitive, pcb_bytes_registers);

    log_warning(logger, "Recibiendo PCB con PID: %d", recived_pcb->pid);

    // Recibo bool para saber si tengo que recibir instrucciones
    void * bytes_recibo_solicitud = list_get(lista_pcb_bytes, 2);
    bool recibo_solititud = deserializar_int(bytes_recibo_solicitud);
    log_warning(logger, "Recibiendo PCB con solicitud de IO: [%s]", recibo_solititud? "true" : "false");

    if(!recibo_solititud){
        recived_pcb->solicitud_io = NULL;

        free(lista_pcb_bytes);
        free(bytes_recibo_solicitud);

        return recived_pcb;
    }
    else {
        char** tokens_instr = deserializar_lista_strings(lista_pcb_bytes, 3);

        // Recibo bool para saber si tengo que recibir peticiones
        void * bytes_recibo_peticiones = list_get(lista_pcb_bytes, 4 + string_array_size(tokens_instr));
        bool recibo_peticiones = deserializar_int(bytes_recibo_peticiones);
        log_warning(logger, "Recibiendo PCB con peticioens de IO: [%s]", recibo_peticiones? "true" : "false");

        // Si no tengo que recibir peticiones, seteo las peticiones en NULL
        t_list* peticiones_memoria = NULL;
        if(recibo_peticiones){
            peticiones_memoria = peticiones_desempaquetar_segun_index(lista_pcb_bytes, 5 + string_array_size(tokens_instr));
            log_warning(logger, "Desempaqueteando peticiones de memoria");
        }

        solicitud_bloqueo_por_io* solicitud_recibida = crear_solicitud_io(tokens_instr, peticiones_memoria);
        recived_pcb->solicitud_io = solicitud_recibida;
        free(bytes_recibo_peticiones);
    }
        
    // Faltan frees
    free(lista_pcb_bytes);
    free(bytes_recibo_solicitud);

    return recived_pcb;
}

void* serializar_pcb(pcb* pcb){
    void* stream = malloc(pcb_size()); 
    int offset = 0;

    // Datos primitivos dentro del PCB
    memcpy(stream + offset, &(pcb->pid), sizeof(unsigned));
    offset += sizeof(unsigned);
    memcpy(stream + offset, &(pcb->pc), sizeof(unsigned));
    offset += sizeof(unsigned);
    memcpy(stream + offset, &(pcb->quantum), sizeof(unsigned));
    offset += sizeof(unsigned);
    memcpy(stream + offset, &(pcb->estado), sizeof(state));
    offset += sizeof(state);

    // Datos de los registros
    memcpy(stream + offset, &(pcb->registros->ax), sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(stream + offset, &(pcb->registros->bx), sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(stream + offset, &(pcb->registros->cx), sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(stream + offset, &(pcb->registros->dx), sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(stream + offset, &(pcb->registros->eax), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &(pcb->registros->ebx), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &(pcb->registros->ecx), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &(pcb->registros->edx), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &(pcb->registros->si), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &(pcb->registros->di), sizeof(uint32_t));
    offset += sizeof(uint32_t);

    return stream;
}


void* serializar_pcb_data_primitive(pcb* pcb){
    void* stream = malloc(size_primitive_data); 
    int offset = 0;

    // Datos primitivos dentro del PCB
    memcpy(stream + offset, &(pcb->pid), sizeof(unsigned));
    offset += sizeof(unsigned);
    memcpy(stream + offset, &(pcb->pc), sizeof(unsigned));
    offset += sizeof(unsigned);
    memcpy(stream + offset, &(pcb->quantum), sizeof(unsigned));
    offset += sizeof(unsigned);
    memcpy(stream + offset, &(pcb->estado), sizeof(state));
    offset += sizeof(state);

    return stream;
}

void* serializar_registros(registros_t* registros){
    void* stream = malloc(size_registros); 
    int offset = 0;

  // Datos de los registros
    memcpy(stream + offset, &(registros->ax), sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(stream + offset, &(registros->bx), sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(stream + offset, &(registros->cx), sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(stream + offset, &(registros->dx), sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(stream + offset, &(registros->eax), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &(registros->ebx), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &(registros->ecx), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &(registros->edx), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &(registros->si), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &(registros->di), sizeof(uint32_t));
    offset += sizeof(uint32_t);

    return stream;
}


pcb* deserializar_pcb_new(void* pcb_bytes){
    pcb* recived_pcb = (pcb*) malloc(sizeof(pcb));
    int offset = 0;

    // Datos con tipo de dato primitivo dentro del PCB
    memcpy(&(recived_pcb->pid), pcb_bytes + offset, sizeof(unsigned));
    offset += sizeof(unsigned);
    memcpy(&(recived_pcb->pc), pcb_bytes + offset, sizeof(unsigned));
    offset += sizeof(unsigned);
    memcpy(&(recived_pcb->quantum), pcb_bytes + offset, sizeof(unsigned));
    offset += sizeof(unsigned);
    memcpy(&(recived_pcb->estado), pcb_bytes + offset, sizeof(state));
    offset += sizeof(state);

    // Datos de los registros
    registros_t* recived_registers = (registros_t*) malloc(sizeof(registros_t));

    // Datos de los registros
    memcpy(&(recived_registers->ax), pcb_bytes + offset, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(&(recived_registers->bx), pcb_bytes + offset, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(&(recived_registers->cx), pcb_bytes + offset, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(&(recived_registers->dx), pcb_bytes + offset, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(&(recived_registers->eax), pcb_bytes + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&(recived_registers->ebx), pcb_bytes + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&(recived_registers->ecx), pcb_bytes + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&(recived_registers->edx), pcb_bytes + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&(recived_registers->si), pcb_bytes + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&(recived_registers->di), pcb_bytes + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    recived_pcb->registros = recived_registers;

    free(pcb_bytes);

    return recived_pcb;
}

pcb* deserializar_pcb(void* pcb_data_primitive, void* pcb_data_registers){
    pcb* recived_pcb = (pcb*) malloc(sizeof(pcb));
    int offset = 0;

    // Datos con tipo de dato primitivo dentro del PCB
    memcpy(&(recived_pcb->pid), pcb_data_primitive + offset, sizeof(unsigned));
    offset += sizeof(unsigned);
    memcpy(&(recived_pcb->pc), pcb_data_primitive + offset, sizeof(unsigned));
    offset += sizeof(unsigned);
    memcpy(&(recived_pcb->quantum), pcb_data_primitive + offset, sizeof(unsigned));
    offset += sizeof(unsigned);
    memcpy(&(recived_pcb->estado), pcb_data_primitive + offset, sizeof(state));
    offset += sizeof(state);

    // Datos de los registros
    recived_pcb->registros = deserializar_registros(pcb_data_registers);

    free(pcb_data_primitive);
    free(pcb_data_registers);

    return recived_pcb;
}

registros_t* deserializar_registros(void* registros){
    registros_t* recived_registers = (registros_t*) malloc(sizeof(registros_t));
    int offset = 0;

    // Datos de los registros
    memcpy(&(recived_registers->ax), registros + offset, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(&(recived_registers->bx), registros + offset, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(&(recived_registers->cx), registros + offset, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(&(recived_registers->dx), registros + offset, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(&(recived_registers->eax), registros + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&(recived_registers->ebx), registros + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&(recived_registers->ecx), registros + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&(recived_registers->edx), registros + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&(recived_registers->si), registros + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&(recived_registers->di), registros + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    return recived_registers;
}

void loggear_pcb(pcb* pcb){
    log_info(logger, "==========PCB==========");
    log_info(logger, "PID: %d", pcb->pid);
    log_info(logger, "Estado: %c", pcb->estado);
    log_info(logger, "PC: %d", pcb->pc);
    log_info(logger, "Quantum: %d", pcb->quantum);
    log_info(logger, "Registros:");
    log_info(logger, "AX: %d", pcb->registros->ax);
    log_info(logger, "BX: %d", pcb->registros->bx);
    log_info(logger, "CX: %d", pcb->registros->cx);
    log_info(logger, "DX: %d", pcb->registros->dx);
    log_info(logger, "EAX: %d", pcb->registros->eax);
    log_info(logger, "EBX: %d", pcb->registros->ebx);
    log_info(logger, "ECX: %d", pcb->registros->ecx);
    log_info(logger, "EDX: %d", pcb->registros->edx);
    log_info(logger, "SI: %d", pcb->registros->si);
    log_info(logger, "DI: %d", pcb->registros->di);
    log_info(logger, "====================");
}

pcb* esperar_pcb(int socket, op_code codigo_esperado){
	int codigo_recibido = recibir_operacion(socket);
	log_info(logger, "Esperar_respuesta: Codigo: [%d]", codigo_recibido);
    if(codigo_recibido == codigo_esperado) {
        return recibir_pcb(socket);
    }

    log_error(logger, "El código de respuesta no es el esperado. Esperado: [%d]. Recibido: [%d]", codigo_esperado, codigo_recibido);

    return NULL;
}

void destruir_pcb(pcb* pcb){
    free(pcb->registros);
    free(pcb);
}

void siguiente_pc(pcb* pcb){
    pcb->pc++;
}