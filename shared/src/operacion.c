#include "../include/operacion.h"

const table_element tabla_op[] = {
    {"SET", SET},
    {"MOV_IN", MOV_IN},
    {"MOV_OUT", MOV_OUT},
    {"SUM", SUM},
    {"SUB", SUB},
    {"JNZ", JNZ},
    {"RESIZE", RESIZE},
    {"COPY_STRING", COPY_STRING},
    {"WAIT", WAIT},
    {"SIGNAL", SIGNAL},
    {"IO_GEN_SLEEP", IO_GEN_SLEEP},
    {"IO_STDIN_READ", IO_STDIN_READ},
    {"IO_STDOUT_WRITE", IO_STDOUT_WRITE},
    {"IO_FS_CREATE", IO_FS_CREATE},
    {"IO_FS_DELETE", IO_FS_DELETE},
    {"IO_FS_TRUNCATE", IO_FS_TRUNCATE},
    {"IO_FS_WRITE", IO_FS_WRITE},
    {"IO_FS_READ", IO_FS_READ},
    {"EXIT", EXIT_OP}
};

operacion get_operacion(char** inst_tokenizada) {
    unsigned table_size = sizeof(tabla_op) / sizeof(table_element);
    get_table_value(inst_tokenizada[0], tabla_op, table_size);
}

int get_valor(char* token_valor){
    return atoi(token_valor);
}

bool io_tiene_operacion(int* operaciones_io, char* operacion_token, io_tipo tipo_io){
    operacion op_a_revisar = get_operacion(&operacion_token);
    if(!tipo_io == DIALFS){
        return  operaciones_io[0] == op_a_revisar 
        ||      operaciones_io[1] == op_a_revisar
        ||      operaciones_io[2] == op_a_revisar
        ||      operaciones_io[3] == op_a_revisar
        ||      operaciones_io[4] == op_a_revisar;
    }
    else{
        return operaciones_io[0] == op_a_revisar;
    }
}

void log_operaciones(int* operaciones_io, io_tipo tipo_io){
    if(!tipo_io == DIALFS){
        log_info(logger, "Operaciones IO: [%d] [%d] [%d] [%d] [%d]", operaciones_io[0], operaciones_io[1], operaciones_io[2], operaciones_io[3], operaciones_io[4]);
    }
    else{
        log_info(logger, "Operaciones IO: [%d]", operaciones_io[0]);
    }
}

const operacion tabla_op_mem[] = {
 MOV_IN,
 MOV_OUT,
 COPY_STRING,
 IO_STDIN_READ,
 IO_STDOUT_WRITE,
};

bool operacion_usa_memoria(char* token_operacion){
    operacion op_consultada = get_operacion(&token_operacion);
    for(int i = 0; i < sizeof(tabla_op_mem) / sizeof(operacion); i++){
        if(tabla_op_mem[i] == op_consultada){
            return 1;
        }
        else{
            return 0;
        }
    }
}