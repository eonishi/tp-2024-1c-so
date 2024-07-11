#include "../include/proceso_mem.h"

t_list *procesos_en_memoria;

void inicializar_procesos_en_memoria(){
    procesos_en_memoria = list_create();
}

static t_list *leer_archivo_instrucciones(char *file_name)
{
    t_list *instrucciones = list_create();

    char *path = string_new();
    string_append_with_format(&path, "%s%s", config.path_instrucciones, file_name);
    log_info(logger, "Leer_archivo_instr path: [%s]", path);

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
        string_n_append(&mi_linea, linea, string_length(linea));

        // Remueve todos los caracteres vacios de la derecha y la izquierda
        string_trim(&mi_linea);
        if(string_length(mi_linea) < 4) continue;

        list_add(instrucciones, mi_linea);
    }

    fclose(archivo);
    free(path);
    free(linea);

    return instrucciones; 
    // Lista de instrucciones individuales de todo el archivo: ["SET AX 3","SUM AX BX", "RESIZE 89"]
}

void cargar_proceso_en_memoria(char* path, unsigned PID){
    t_proceso_en_memoria* nuevo_set_instruc = malloc(sizeof(t_proceso_en_memoria));
    nuevo_set_instruc->PID = PID;
    nuevo_set_instruc->instrucciones = leer_archivo_instrucciones(path);
    nuevo_set_instruc->tabla_paginas = list_create();

    //Guardo en una lista los procesos en memoria (Temporal hasta definir como se guardan)
    list_add(procesos_en_memoria, nuevo_set_instruc);
    log_info(logger, "Instrucciones del proceso PID:%u cargadas en memoria", PID);
} 

// ----Condiciones de búsqueda----
unsigned PID_a_liberar; // Solo deber ser asignada por liberar_instr_set 
unsigned PID_solicitado; // revisar si necesitaria mutex (no creo).

bool memoria_tiene_pid(void* set_instrucciones, unsigned PID){
    return ((t_proceso_en_memoria*)set_instrucciones)->PID == PID;
}
bool memoria_tiene_pid_solicitado(void* set_instrucciones){
    return memoria_tiene_pid(set_instrucciones, PID_solicitado);
}
static bool memoria_tiene_pid_a_liberar(void* set_instrucciones){
    return memoria_tiene_pid(set_instrucciones, PID_a_liberar);
}//--------

t_proceso_en_memoria* get_proceso_by_PID(unsigned PID, unsigned* PID_ptr, bool memoria_tiene_pid(void*)){
    *PID_ptr = PID;
    return list_find(procesos_en_memoria, memoria_tiene_pid);
}

// ----Liberador de memoria----
static void proceso_mem_destroyer(void* set_instrucciones){
    t_proceso_en_memoria* proceso_a_destruir = (t_proceso_en_memoria*)set_instrucciones;
    log_info(logger, "Liberando proceso PID:%u", proceso_a_destruir->PID);

    log_info(logger, "Por eliminar %d instrucciones", list_size(proceso_a_destruir->instrucciones));
    list_destroy_and_destroy_elements(proceso_a_destruir->instrucciones, free);

    size_t cantidad_paginas = list_size(proceso_a_destruir->tabla_paginas);
    log_info(logger, "Por eliminar %d paginas", cantidad_paginas);
    quitar_paginas(cantidad_paginas, proceso_a_destruir);

    log_info(logger, "Proceso PID:%u liberado", proceso_a_destruir->PID);
    free(proceso_a_destruir);
}

void liberar_proceso_en_memoria(unsigned PID){
    PID_a_liberar=PID;
    list_remove_and_destroy_by_condition(procesos_en_memoria, memoria_tiene_pid_a_liberar, proceso_mem_destroyer);
}//--------