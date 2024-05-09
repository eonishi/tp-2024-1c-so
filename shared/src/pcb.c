#include "../include/pcb.h"

// Tamaño de los registros y PCB
const int size_registros = sizeof(uint8_t) * 4 + sizeof(uint32_t) * 6;
const int size_primitive_data = sizeof(unsigned) * 3;
const int size_pcb = size_primitive_data + size_registros;

pcb* crear_pcb(unsigned id, unsigned quantum){
    pcb* nuevo_pcb = (pcb*) malloc(sizeof(pcb));
    nuevo_pcb->registros = (registros_t*) malloc(sizeof(registros_t));

    nuevo_pcb->pid = id;
    nuevo_pcb->quantum = quantum;
    nuevo_pcb->pc = 0;
    nuevo_pcb->registros = crear_registros();
    nuevo_pcb->estado = 'N';
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

int enviar_pcb(pcb* pcb, int socket_cliente, op_code code){
    t_paquete* paquete = crear_paquete(code);

    void* data_primitive = serializar_pcb_data_primitive(pcb); // Datos con tipos de datos primitivos de C dentro del PCB
    void* registros = serializar_registros(pcb->registros);
    
    agregar_a_paquete(paquete, data_primitive, size_pcb);
    agregar_a_paquete(paquete, registros, size_registros);

    enviar_paquete(paquete, socket_cliente);

    free(data_primitive);
    free(registros);
    free(paquete);

    return 0;
}

pcb* recibir_pcb(int socket_cliente){
    t_list* lista_pcb_bytes = recibir_paquete(socket_cliente);

    void* pcb_bytes_primitive = list_get(lista_pcb_bytes, 0);
    void* pcb_bytes_registers = list_get(lista_pcb_bytes, 1);

    pcb* recived_pcb = deserializar_pcb(pcb_bytes_primitive, pcb_bytes_registers);

    free(lista_pcb_bytes);
    return recived_pcb;
}

void* serializar_pcb_data_primitive(pcb* pcb){
    void* stream = malloc(size_pcb); 
    int offset = 0;

    // Datos primitivos dentro del PCB
    memcpy(stream + offset, &(pcb->pid), sizeof(unsigned));
    offset += sizeof(unsigned);
    memcpy(stream + offset, &(pcb->pc), sizeof(unsigned));
    offset += sizeof(unsigned);
    memcpy(stream + offset, &(pcb->quantum), sizeof(unsigned));
    offset += sizeof(unsigned);

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