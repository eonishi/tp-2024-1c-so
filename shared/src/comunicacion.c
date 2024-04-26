#include "../include/comunicacion.h"

void enviar_handshake(int socket) {
    enviar_mensaje(HANDSHAKE, "HANDSHAKE", socket);
}

int esperar_handshake(int socket) {
    int cod_op = recibir_operacion(socket);

    if(cod_op == HANDSHAKE) {
        recibir_mensaje(socket);

        return 0;
    }

    return ERROR;
}