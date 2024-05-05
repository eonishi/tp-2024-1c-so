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
        list_add(instrucciones, mi_linea);
    }
    fclose(archivo);

    return instrucciones; 
    // Lista de instrucciones individuales de todo el archivo: ["SET AX 3","SUM AX BX", "RESIZE 89"]
}

void crear_instr_set(char* path, unsigned PID){
    t_InstrSet* nuevo_set_instruc = malloc(sizeof(t_InstrSet));
    nuevo_set_instruc->PID = PID;
    nuevo_set_instruc->instrucciones = leer_archivo_instrucciones(path);

    //Guardo en una lista los procesos en memoria (Temporal hasta definir como se guardan)
    list_add(procesos_en_memoria, nuevo_set_instruc);
} 

char* get_instr_by_pc(unsigned PID, unsigned PC){
    t_InstrSet* set_buscado = list_get(procesos_en_memoria, PID);  // el PID coincide con el index en la lista

    if(PC >= list_size(set_buscado->instrucciones)){
        log_error(logger, "No hay más instrucciones del proceso PID:%u", PID);
        return "OUT OF BOUNDS"; // Hacer un caso "FAIL" en decode o chequear antes de enviar a CPU
    }
    char* instruc_buscada = list_get(set_buscado->instrucciones, PC);

    return instruc_buscada;
}

void enviar_instruccion_a_cpu(char* instruccion){
    enviar_mensaje(FETCH_INSTRUCCION,instruccion, socket_cpu);
}

unsigned recibir_solicitud_de_cpu(){
    // es la misma implementacion que recibir_mensaje() pero devuelve el PC
    int size;
    char *buffer = recibir_buffer(&size, socket_cpu);
    log_info(logger, "Mensaje recibido: [%s]", buffer);
    return atoi(buffer);
}