#include "process_info.h"
#include <stdio.h>    // Para FILE, fopen, fgets, etc.
#include <stdlib.h>   // Para sscanf
#include <string.h>   // Para strncmp
#include <ctype.h>

int get_process_info(pid_t pid, struct process_info *info) {
    char path[256];
    sprintf(path, "/proc/%d/status", pid);  // Construye ruta al archivo
    
    FILE *file = fopen(path, "r");  // Abrir el archivo en modo lectura
    if (!file) {
        return -1;   // Error si no se puede abrir
    }

    info->pid = pid;  // Asigna el PID recibido
    char line[256];
    
    // Leemos el archivo línea por línea
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "Name:", 5) == 0) {
            sscanf(line, "Name:\t%s", info->name);
        } else if (strncmp(line, "State:", 6) == 0) {
            sscanf(line, "State:\t%c", &info->state);
        } else if (strncmp(line, "VmSize:", 7) == 0) {
            sscanf(line, "VmSize:\t%lu", &info->vm_size);
        } else if (strncmp(line, "VmExe:", 6) == 0) {
            sscanf(line, "VmExe:\t%lu", &info->vm_exe);
        } else if (strncmp(line, "VmData:", 7) == 0) {
            sscanf(line, "VmData:\t%lu", &info->vm_data);
        } else if (strncmp(line, "VmStk:", 6) == 0) {
            sscanf(line, "VmStk:\t%lu", &info->vm_stk);
        } else if (strncmp(line, "voluntary_ctxt_switches:", 24) == 0) {
            sscanf(line, "voluntary_ctxt_switches:\t%lu", &info->voluntary_ctxt_switches);
        } else if (strncmp(line, "nonvoluntary_ctxt_switches:", 27) == 0) {
            sscanf(line, "nonvoluntary_ctxt_switches:\t%lu", &info->nonvoluntary_ctxt_switches);
        }
    }
    
    fclose(file);  //Cierra el archivo
    return 0;
}

void print_process_info(const struct process_info *info) {
    printf("Pid: %d\n", info->pid);
    printf("Nombre del proceso: %s\n", info->name);
    printf("Estado: %c\n", info->state);
    printf("Tamaño total de la imagen de memoria: %lu KB\n", info->vm_size);
    printf("Tamaño de la memoria TEXT: %lu KB\n", info->vm_exe);
    printf("Tamaño de la memoria DATA: %lu KB\n", info->vm_data);
    printf("Tamaño de la memoria STACK: %lu KB\n", info->vm_stk);
    printf("Número de cambios de contexto (voluntarios - no voluntarios): %lu - %lu\n",
           info->voluntary_ctxt_switches, info->nonvoluntary_ctxt_switches);
}