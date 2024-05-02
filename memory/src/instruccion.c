#include "../include/instruccion.h"

t_list *leer_archivo_instrucciones(char *path)
{
    t_list *instrucciones = list_create();

    FILE *archivo = fopen(path, "r");
    if (archivo == NULL){
        log_error(logger, "Error al abrir el archivo de instrucciones");
        exit(EXIT_FAILURE); // A checkear el exit 😅
    }

    char *linea = NULL; // NULL es importante para que getline haga el malloc automaticamente
    size_t len = 0;     // len = 0 es importante para que getline haga el malloc automaticamente
    ssize_t read;
    while ((read = getline(&linea, &len, archivo)) != -1)
    {
        char *mi_linea = string_new();
        string_n_append(&mi_linea, linea, string_length(linea) - 1);
        list_add(instrucciones, string_split(mi_linea, " "));
    }
    fclose(archivo);

    return instrucciones; 
    // Lista de instrucciones individuales [["SET", "AX", "3"],["SUM", "AX", "BX"],["RESIZE", "89"]]
}

t_InstrSet* crear_instr_set(char* path, unsigned PID){
    t_InstrSet* nuevo_set_instruc = malloc(sizeof(t_InstrSet));
    nuevo_set_instruc->PID = PID;
    nuevo_set_instruc->instrucciones = leer_archivo_instrucciones(path);
    return nuevo_set_instruc;
} 
