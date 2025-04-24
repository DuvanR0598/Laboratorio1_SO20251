#include "process_info.h"
#include <stdio.h>    // Para imprimir mensajes de error y salida
#include <stdlib.h>   // Para funciones como atoi (convertir texto a número)
#include <string.h>   // Para trabajar con cadenas de texto
#include <ctype.h>    // Para verificar si un carácter es numérico

int main(int argc, char *argv[]) {
    //Validacion de argumentos 
    if (argc < 2) {
        fprintf(stderr, "Error: Argumentos insuficientes\n");
        print_usage(argv[0]);
        return ERR_INVALID_ARGS;
    }

    // Inicializar variables
    int use_list_mode = 0;    // Bandera para detectar si usamos el modo -l
    int use_report_mode = 0;  // Bandera para detectar opcion -r
    psinfo_error_t global_error = PSINFO_OK;  // Variable para rastrear errores globales

    // Procesar argumentos
    if (argv[1][0] == '-'){
        if (strcmp(argv[1], "-l") == 0) {  //Verificamos si el primer argumento es -l
            use_list_mode = 1;
        }else if (strcmp(argv[1], "-r") == 0){
            use_report_mode = 1;
        } else {
            fprintf(stderr, "Error: Opción inválida '%s'\n", argv[1]);
            print_usage(argv[0]);
            return ERR_INVALID_ARGS; 
        }

        // Verificar que haya al menos un PID después de la opción
        if (argc < 3) {
            fprintf(stderr, "Error: Se requiere al menos un PID con la opción %s\n", argv[1]);
            print_usage(argv[0]);
            return ERR_NO_PIDS;
        }
    }

    // Crear lista dinámica
    struct process_list *processes = create_process_list();
    if (!processes) {
        fprintf(stderr, "Error: No se pudo asignar memoria\n");
        return ERR_MEMORY;
    }

    //Validar y recolectar PIDs validos
    for (int i = (use_list_mode || use_report_mode) ? 2 : 1; i < argc; i++) {
        // Verificar que el argumento sea numérico
        for (char *p = argv[i]; *p != '\0'; p++) {
            if (!isdigit(*p)) {
                fprintf(stderr, "Error: PID inválido '%s' (debe ser numérico)\n", argv[i]);
                global_error = ERR_INVALID_PID;
                continue;
            }
        }
        
        pid_t pid = atoi(argv[i]);
        if (pid <= 0) {
            fprintf(stderr, "Error: PID inválido '%s' (debe ser positivo)\n", argv[i]);
            global_error = ERR_INVALID_PID;
            continue;
        }

        // Obtener información para cada PID
        struct process_info info;
        psinfo_error_t error = get_process_info(pid, &info);
        if (error != PSINFO_OK) {
            fprintf(stderr, "Error al procesar PID %d: %s\n", pid, error_to_string(error));
            global_error = error;
        } else {
            if (add_to_process_list(processes, &info) != PSINFO_OK) {
                fprintf(stderr, "Error: No hay memoria para almacenar más procesos\n");
                global_error = ERR_MEMORY;
                break;
            }
        }
    }

    if (processes->count == 0) {
        fprintf(stderr, "Error: No se pudo obtener información de ningún proceso\n");
        free_process_list(processes);
        return global_error ? global_error : ERR_PROCESS_INFO;
    }

    // Generar reporte o mostrar información segun opciones
    if (use_report_mode) {
        char filename[MAX_FILENAME] = "psinfo-report";   // filname: Buffer para contruir el nombre del archivo
        
        // Construir nombre de archivo con los PIDs
        for (size_t i = 0; i < processes->count && strlen(filename) < MAX_FILENAME - 10; i++) {
            char pid_str[16];  // Buffer para PID
            snprintf(pid_str, sizeof(pid_str), "-%d", processes->processes[i].pid);
            strncat(filename, pid_str, MAX_FILENAME - strlen(filename) - 1);
        }
        strncat(filename, ".info", MAX_FILENAME - strlen(filename) - 1);

        generate_report(processes, filename); // Generar el reporte
        printf("Archivo de salida generado: %s\n", filename);
    }
    else if (use_list_mode) {
        printf("\n-- Información de procesos recolectada!!!\n");
        for (size_t i = 0; i < processes->count; i++) {
            printf("\nPid: %d\n", processes->processes[i].pid);
            print_process_info(&processes->processes[i]);
        }
    } else {
        printf("\n-- Información del proceso recolectada!!!\n");
        print_process_info(&processes->processes[0]);
    }

    // Liberar recursos
    free_process_list(processes);
    return global_error == PSINFO_OK ? 0 : global_error;
}