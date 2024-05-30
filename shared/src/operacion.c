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
    int i;
    for (i = 0; i < sizeof(tabla_op) / sizeof(table_element); i++) {
        if (string_equals_ignore_case(tabla_op[i].key, inst_tokenizada[0])) {
            return tabla_op[i].value;
        }
    }
    // manejar el caso en que la operación no se reconoce. No va a haber peeeero 🤷🏻‍♂️
    return -1;
}

int get_valor(char* token_valor){
    return atoi(token_valor);
}