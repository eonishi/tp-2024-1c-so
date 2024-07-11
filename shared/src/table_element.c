#include "../include/table_element.h"

int get_table_value(char* clave, const table_element* tabla, unsigned table_size){
    for (int i = 0; i < table_size; i++) {
        if (string_equals_ignore_case(tabla[i].key, clave)) {
            return tabla[i].value;
        }
    }
    return -1;
}