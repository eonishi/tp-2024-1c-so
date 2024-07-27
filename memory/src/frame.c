#include "../include/frame.h"

static void *MEMORIA;
static t_bitarray* FRAME_BITMAP;
static size_t size_bitmap;
pthread_mutex_t mutex_memoria = PTHREAD_MUTEX_INITIALIZER;

unsigned frames_totales(){
    return config.tam_memoria / config.tam_pagina;
}

void imprimir_bitmap(){
    for (size_t i = 0; i < size_bitmap; i++)
    {
        bool valor_del_bit = bitarray_test_bit(FRAME_BITMAP, i);
        log_info(logger, "Bit %d: %d", i, valor_del_bit);
    }
}

void inicializar_bitmap(){
    int cantidad_bits_en_bytes = frames_totales()/8; // 1 byte = 8 bits
    char *bitmap = calloc(cantidad_bits_en_bytes, sizeof(char));
    FRAME_BITMAP = bitarray_create_with_mode(bitmap, cantidad_bits_en_bytes, MSB_FIRST);

    // Esto es test cantidad de frames == cantidad de bits
    size_bitmap = bitarray_get_max_bit(FRAME_BITMAP);
    log_info(logger, "Bitmap de frames creado con %d bits", size_bitmap);
}

void inicializar_memoria(){
    MEMORIA = calloc(config.tam_memoria, sizeof(char));
    inicializar_bitmap();
}

void* get_memoria(uint32_t direccion_fisica){
    // devuelve un puntero a la dirección física.
    return MEMORIA + direccion_fisica;
}

void set_memoria(uint32_t direccion_fisica, void* data, size_t size_data){
    pthread_mutex_lock(&mutex_memoria);
        void* frame = get_memoria(direccion_fisica);
        memcpy(frame, data, size_data);
    pthread_mutex_unlock(&mutex_memoria);

    log_info(logger, "Data [%d] guardada en la direccion [%d]", data, direccion_fisica);

    for(size_t i = 0; i < size_data; i++){
        log_info(logger, "Data byte [%d]: [%c]", i, ((char*)data)[i]);
    }
}

void marcar_frame_como(unsigned frame_number, int estado){
    pthread_mutex_lock(&mutex_memoria);

    if(estado){
        bitarray_set_bit(FRAME_BITMAP, frame_number);
    } else {
        bitarray_clean_bit(FRAME_BITMAP, frame_number);
    }

    pthread_mutex_unlock(&mutex_memoria);
}

unsigned get_available_frame(){
    // Deberia ir un mutex aca?
    for (size_t i = 0; i < size_bitmap; i++){
        if (!bitarray_test_bit(FRAME_BITMAP, i)){
            log_info(logger, "Frame disponible [%d]", i);
            return i;
        }
    }
    log_error(logger, "No hay frames disponibles");
    return -1;
};

unsigned frame_cantidad_estado(int estado){
    unsigned cantidad = 0;
    for (size_t i = 0; i < size_bitmap; i++){
        if (bitarray_test_bit(FRAME_BITMAP, i) == estado){
            cantidad++;
        }
    }
    log_info(logger, "Cantidad de frames %s: %d", estado ? "ocupados" : "libres", cantidad);
    return cantidad;
}

unsigned frames_ocupados(){
    return frame_cantidad_estado(1);
}

unsigned frames_libres(){
    return frame_cantidad_estado(0);
}

bool tengo_espacio_para_agregar(int cantidad_frames){
    return frames_libres() >= cantidad_frames;
}

void imprimir_memoria_hex(){
    mem_hexdump(MEMORIA, config.tam_memoria);
}