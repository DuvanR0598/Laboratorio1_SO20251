#ifndef PROCESS_INFO_H  //Guarda para evitar inclusiones multiples
#define PROCESS_INFO_H

#include <sys/types.h>

#define MAX_PROCESS_NAME 256  //Tamaño maximo para el nombre del proceso
#define MAX_PIDS 20  // Máximo número de PIDs que podemos procesar para evitar desbordamiento
#define MAX_FILENAME 256
#define MAX_ERROR_MSG 512

// Códigos de error
typedef enum {
    PSINFO_OK = 0,         // Operación exitosa
    ERR_INVALID_ARGS = 1,  // Argumentos invalidos
    ERR_NO_PIDS = 2,       // No se especificaron PIDs
    ERR_INVALID_PID = 3,   // PID inavalido o inexistente 
    ERR_FILE_ACCESS = 4,   // Error al acceder a archivos
    ERR_PROCESS_INFO = 5,  // No se pudo obtener informacion del proceso
    ERR_MEMORY = 6         // Error de asignacion de memoria
} psinfo_error_t;

//Estructura que almacena toda la informacion requerida de un proceso
struct process_info {
    pid_t pid;                            // ID del proceso
    char name[MAX_PROCESS_NAME];          // Nombre del proceso
    char state;                           // Estados (S, R, Z...)
    unsigned long vm_size;                // Tamaño total de memoria (KB)
    unsigned long vm_exe;                 // Memoria TEXT (KB)
    unsigned long vm_data;                // Memoria DATA (KB)
    unsigned long vm_stk;                 // Memoria STACK (KB)
    unsigned long voluntary_ctxt_switches;     // Cambios de contexto voluntarios
    unsigned long nonvoluntary_ctxt_switches;  // Cambios de contexto no voluntarios
};

/**
 * Obtiene información de un proceso desde /proc
 * @param pid ID del proceso a consultar
 * @param info Estructura donde se almacenará la información
 * @return 0 en éxito, -1 en error
 */
int get_process_info(pid_t pid, struct process_info *info);

/**
 * Muestra la información del proceso en formato legible
 * @param info Estructura con la información del proceso
 */
void print_process_info(const struct process_info *info);

/**
 * Función para generación de reportes en un archivo
 * @param info_list Array de estructuras con datos de procesos
 * @param count Numero de procesos en el array
 * @param filename nombre del archivo a crear
 */
void generate_report(const struct process_info *info_list, int count, const char *filename);

// Función para manejo de errores
void print_usage(const char *program_name);
const char *error_to_string(psinfo_error_t error);

#endif // PROCESS_INFO_H