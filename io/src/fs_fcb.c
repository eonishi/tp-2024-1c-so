#include "../include/fs_fcb.h"

t_list* fcb_list;

void inicializar_lista_fcb(){
    fcb_list = list_create();
    recuperar_archivos_existentes();
}

void recuperar_archivos_existentes() {
    struct dirent* entry;
    DIR* dir = fs_opendir();

    if (dir == NULL) {
        perror("No se puede abrir el directorio");
        return;
    }

    int recuperados = 0;

    while ((entry = readdir(dir)) != NULL) {
        // Comprobar si el archivo tiene extensión .txt
        if (strstr(entry->d_name, ".txt") != NULL) {
            fcb* new_fcb = (fcb*)malloc(sizeof(fcb));
            if (new_fcb == NULL) {
                perror("Error al asignar memoria para fcb");
                closedir(dir);
                return;
            }

            new_fcb->nombre = strdup(entry->d_name); 
            new_fcb->config = config_create(new_fcb->nombre); 

            insertar_fcb(new_fcb);

            recuperados++;
        }
    }

    log_info(logger, "Archivos recuperados: [%d]", recuperados);

    closedir(dir);
}

void crear_fcb(char* nombre, t_config* config_loader){
    fcb* new_fcb = (fcb*)malloc(sizeof(fcb));
    new_fcb->nombre = nombre;
    new_fcb->config = config_loader;

    list_add(fcb_list, new_fcb);
}

void insertar_fcb(fcb* fcb){
    list_add(fcb_list, fcb);
}

fcb* obtener_fcb(int index){
    list_get(fcb_list, index);
}

int cantidad_de_fcbs(){
    return fcb_list->elements_count;
}

char* nombre_aux;

bool condicion_por_nombre(void* file_control_block){
    return strcmp(((fcb*)file_control_block)->nombre, nombre_aux) == 0;
}

fcb* obtener_fcb_por_nombre(char* nombre){
    nombre_aux = nombre;

    return (fcb*) list_find(fcb_list, condicion_por_nombre);
}

void eliminar_fcb_por_nombre(char* nombre){
    nombre_aux = nombre;
    
    list_remove_by_condition(fcb_list, condicion_por_nombre);
}


void set_campo_de_archivo(char* campo , int valor, t_config* config_loader){
    char valor_char[5];
    sprintf(valor_char, "%d", valor);
    config_set_value(config_loader, campo, valor_char);
    config_save(config_loader);
}

// TODO DESTROY