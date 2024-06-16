#include "../include/frame.h"

static void *MEMORIA;
static t_bitarray* FRAME_BITMAP;
static size_t size_bitmap;

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

void limpiar_bitmap(){
    for (size_t i = 0; i < size_bitmap; i++){
        bitarray_clean_bit(FRAME_BITMAP, i);
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

void* get_frame(int frame_number, int desplazamiento){
    // devuelve un puntero a la base del frame + desplazamiento.
    // Tener en cuenta que se trabaja sobre el espacio de memoria y no sobre una copia de los datos.
    return MEMORIA + (frame_number * config.tam_pagina) + desplazamiento; // tam_pagina = tam_frame en paginación simple
}

void set_frame(int frame_number, int offset, void* data, size_t size_data){
    void* frame = get_frame(frame_number, offset);
    memcpy(frame, data, size_data);
    log_info(logger, "Data [%s] guardada en frame [%d]", data, frame_number);
}

void marcar_frame_como(unsigned frame_number, int estado){
    if(estado){
        bitarray_set_bit(FRAME_BITMAP, frame_number);
    } else {
        bitarray_clean_bit(FRAME_BITMAP, frame_number);
    }
}

unsigned get_available_frame(){
    for (size_t i = 0; i < size_bitmap; i++){
        if (!bitarray_test_bit(FRAME_BITMAP, i)){
            log_info(logger, "Frame disponible [%d]", i);
            return i;
        }
    }
    log_error(logger, "No hay frames disponibles");
    return -1;
};

void imprimir_data(int frame_number, int offset, size_t value_size){
    void* frame = get_frame(frame_number, offset);
    char* value = malloc(value_size);
    memcpy(value, frame, value_size);
    log_info(logger, "Data en frame [%d]: [%s]", frame_number, value);
}

void  imprimir_frames(){
    for (size_t i = 0; i < size_bitmap; i++){
        void* frame = get_frame(i, 0);
        char* frame_value = malloc(config.tam_pagina);
        memcpy(frame_value, frame, config.tam_pagina);
        log_info(logger, "Frame [%d]: [%s] esta [%s]", i, frame_value, bitarray_test_bit(FRAME_BITMAP, i) ? "Ocupado" : "Libre");
        free(frame_value);
    }
}

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
