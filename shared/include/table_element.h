#ifndef TABLE_ELEMENT_H_
#define TABLE_ELEMENT_H_

#include <commons/string.h>

// Pseudo diccionario estatico para poder mapear estaticamente nombres con enum
typedef struct {
    char* key;
    int value;
} table_element; 

int get_table_value(char* key, const table_element* table, unsigned table_size);

#endif