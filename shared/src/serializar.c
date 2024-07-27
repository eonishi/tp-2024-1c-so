#include "../include/serializar.h"

void* serializar_int8(int8_t value){
    void* stream = malloc(sizeof(int8_t)); 

    memcpy(stream, &value, sizeof(int8_t));
    
    return stream;
}

void* serializar_int(int value){
    void* stream = malloc(sizeof(int)); 

    memcpy(stream, &value, sizeof(int));
    
    return stream;
}

int8_t deserializar_int8(void* int_bytes){
    int8_t* result =malloc(sizeof(int8_t));
    int offset = 0;

    memcpy(result, int_bytes, sizeof(int8_t));

    return *result;
}

int deserializar_int(void* int_bytes){
    int* result =malloc(sizeof(int));
    memcpy(result, int_bytes, sizeof(int));

    //free(int_bytes); Falta un free de los bytes que se reciben por parametro
    return *result;
}


void* serializar_char(char* string){
    size_t size = strlen(string) + 1;
    void* stream = malloc(size); 

    memcpy(stream, string, size);
    
    return stream;
}

char* deserializar_char(void* char_bytes, int8_t size){
    char* string = (char*) malloc(size);
    int offset = 0;

    memcpy(string, char_bytes, size);

    return string;
}

void* serializar_uint32(uint32_t value){
    void* stream = malloc(sizeof(uint32_t)); 

    memcpy(stream, &value, sizeof(uint32_t));
    
    return stream;
}

uint32_t deserializar_uint32(void* int_bytes){
    uint32_t* result =malloc(sizeof(uint32_t));
    memcpy(result, int_bytes, sizeof(uint32_t));

    return *result;
}

// Usar siempre como última serializacion. Ejemplo: {pcb: "", lista_string: []}
void serializar_lista_strings_y_agregar_a_paquete(char** lista_strings, t_paquete* paquete){

    void* cantidad_tokens = serializar_int(string_array_size(lista_strings));

    agregar_a_paquete(paquete, cantidad_tokens, sizeof(int));

    for(int index = 0 ; index < string_array_size(lista_strings); index++){
        void* token = serializar_char(lista_strings[index]);

        agregar_a_paquete(paquete, token, strlen(lista_strings[index]) + 1);
    }
}

char** deserializar_lista_strings(t_list* bytes, int index_cantidad_tokens){ 
    int cantidad_tokens = deserializar_int(list_get(bytes, index_cantidad_tokens)); 

    char** tokens = string_array_new();
    
    for(int i = index_cantidad_tokens+1;i < cantidad_tokens+index_cantidad_tokens+1; i++){
        char* token = list_get(bytes, i);

        string_array_push(&tokens, token);
    }    

    return tokens;
}

void liberar_lista_strings(char** lista_strings){
    if(lista_strings == NULL) return;
    for(int index = 0; index < string_array_size(lista_strings); index++){
        free(lista_strings[index]);
    }

    free(lista_strings);
}