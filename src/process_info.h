#ifndef PROCESS_INFO_H  //Guarda para evitar inclusiones multiples
#define PROCESS_INFO_H

#include <sys/types.h>

#define MAX_PROCESS_NAME 256  //Tamaño maximo para el nombre del proceso

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
 * DECLARACION DE FUNCIONES
 * Obtiene información de un proceso desde /proc
 * @param pid ID del proceso
 * @param info Estructura donde se almacenará la información
 * @return 0 en éxito, -1 en error
 */
int get_process_info(pid_t pid, struct process_info *info);

/**
 * Muestra la información del proceso en formato legible
 * @param info Estructura con la información del proceso
 */
void print_process_info(const struct process_info *info);

#endif // PROCESS_INFO_H