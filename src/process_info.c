#include "process_info.h"
#include <stdio.h>    // Para FILE, fopen, fgets, etc.
#include <stdlib.h>   // Para sscanf
#include <string.h>   // Para strncmp
#include <ctype.h>
#include <time.h>
#include <errno.h>

// Mapeo de códigos de error a mensajes
static const char *error_messages[] = {
    "Operación exitosa",
    "Argumentos inválidos",
    "No se especificaron PIDs",
    "PID inválido o inexistente",
    "Error al acceder a archivos del sistema",
    "No se pudo obtener información del proceso",
    "Error de asignación de memoria"
};

const char *error_to_string(psinfo_error_t error) {
    if (error >= 0 && error <= ERR_MEMORY) {
        return error_messages[error];
    }
    return "Error desconocido";
}

void print_usage(const char *program_name) {
    fprintf(stderr, "Uso correcto:\n");
    fprintf(stderr, "  %s <PID>                       # Muestra información de un proceso\n", program_name);
    fprintf(stderr, "  %s -l <PID1> <PID2> ...       # Muestra información de múltiples procesos\n", program_name);
    fprintf(stderr, "  %s -r <PID1> <PID2> ...       # Genera reporte en archivo\n", program_name);
    fprintf(stderr, "\nEjemplos:\n");
    fprintf(stderr, "  %s 1234\n", program_name);
    fprintf(stderr, "  %s -l 1234 5678\n", program_name);
    fprintf(stderr, "  %s -r 1234 5678\n", program_name);
}

int get_process_info(pid_t pid, struct process_info *info) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/status", pid);  // Construye ruta al archivo
    
    FILE *file = fopen(path, "r");  // Abrir el archivo en modo lectura
    if (!file) {
        if(errno == ENOENT){
            return ERR_INVALID_PID;  // El proceso no existe
        }
        return ERR_FILE_ACCESS;  // Otro error de acceso
    }

    // Inicializar estructura
    memset(info, 0, sizeof(struct process_info));
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

    // Validar que se obtuvieron todos los campos necesarios
    if (info->name[0] == '\0') {
        return ERR_PROCESS_INFO;
    }
    
    return PSINFO_OK;
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

/* GENERAR UN ARCHIVO DE REPORTE
 * @param info_list Array de estructuras con información de procesos
 * @param count Cantidad de procesos en el array
 * @param filename Nombre del archivo a generar
 */
void generate_report(const struct process_info *info_list, int count, const char *filename) {
    FILE *report = fopen(filename, "w");
    if (!report) {
        perror("Error al crear archivo de reporte");
        return;
    }

    // Escribir la cabecera del reporte
    time_t now = time(NULL);
    fprintf(report, "Reporte generado el: %s\n", ctime(&now));
    fprintf(report, "=================================\n\n");

    // Escribir información de cada proceso
    for (int i = 0; i < count; i++) {
        fprintf(report, "Pid: %d\n", info_list[i].pid);
        fprintf(report, "Nombre del proceso: %s\n", info_list[i].name);
        fprintf(report, "Estado: %c\n", info_list[i].state);
        fprintf(report, "Tamaño total de memoria: %lu KB\n", info_list[i].vm_size);
        fprintf(report, "Tamaño TEXT: %lu KB\n", info_list[i].vm_exe);
        fprintf(report, "Tamaño DATA: %lu KB\n", info_list[i].vm_data);
        fprintf(report, "Tamaño STACK: %lu KB\n", info_list[i].vm_stk);
        fprintf(report, "Cambios de contexto (V-NV): %lu - %lu\n\n",
                info_list[i].voluntary_ctxt_switches,
                info_list[i].nonvoluntary_ctxt_switches);
    }

    fclose(report);
}