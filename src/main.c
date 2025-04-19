#include "process_info.h"
#include <stdio.h>    // Para fprintf
#include <stdlib.h>   // Para atoi
#include <string.h>
#include <ctype.h>

//#define MAX_PIDS 20  // Máximo número de PIDs que podemos procesar para evitar desbordamiento

int main(int argc, char *argv[]) {
    //Validacion de argumentos 
    if (argc < 2) {
        fprintf(stderr, "Error: Argumentos insuficientes\n");
        print_usage(argv[0]);
        return ERR_INVALID_ARGS;
    }

    int use_list_mode = 0;    // Flag para detectar si usamos el modo -l
    int use_report_mode = 0;  //Flag para detectar opcion -r
    int pid_count = 0;        // Contador de PIDs proporcionados
    pid_t pids[MAX_PIDS];     // Array para almacenar PIDs a procesar 
    psinfo_error_t global_error = PSINFO_OK;

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

        if (argc < 3) {
            fprintf(stderr, "Error: Se requiere al menos un PID con la opción %s\n", argv[1]);
            print_usage(argv[0]);
            return ERR_NO_PIDS;
        }
    }

    //Validar y recolectar PIDs
    for (int i = (use_list_mode || use_report_mode) ? 2 : 1; i < argc && pid_count < MAX_PIDS; i++) {
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
        
        pids[pid_count++] = pid;
    }

    if (pid_count == 0) {
        fprintf(stderr, "Error: No se proporcionaron PIDs válidos\n");
        return ERR_NO_PIDS;
    }

    struct process_info info_list[MAX_PIDS];  // Almacenar info de todos los procesos
    int success_count = 0;                    // Para llevar el registro de PIDs procesados correctamente 

    // Obtener información para cada PID
    for (int i = 0; i < pid_count; i++) {
        psinfo_error_t error = get_process_info(pids[i], &info_list[success_count]);
        if (error != PSINFO_OK) {
            fprintf(stderr, "Error al procesar PID %d: %s\n", 
                    pids[i], error_to_string(error));
            global_error = error;
        } else {
            success_count++;
        }
    }

    if (success_count == 0) {
        fprintf(stderr, "Error: No se pudo obtener información de ningún proceso\n");
        return global_error ? global_error : ERR_PROCESS_INFO;
    }

    // Generar reporte o mostrar información segun opciones
    if (use_report_mode) {
        char filename[MAX_FILENAME] = "psinfo-report";   // filname: Buffer para contruir el nombre del archivo
        
        // Construir nombre de archivo con los PIDs
        for (int i = 0; i < success_count; i++) {
            char pid_str[10];
            sprintf(pid_str, "-%d", info_list[i].pid);
            strncat(filename, pid_str, MAX_FILENAME - strlen(filename) - 1);
        }
        strncat(filename, ".info", MAX_FILENAME - strlen(filename) - 1);

        generate_report(info_list, success_count, filename);
        printf("Archivo de salida generado: %s\n", filename);
    }

    // Mostrar en pantalla si no es modo reporte o si es modo lista
    else if (!use_report_mode) {
        if (use_list_mode) {
            printf("\n-- Información recolectada!!!\n");
            for (int i = 0; i < success_count; i++) {
                printf("\nPid: %d\n", info_list[i].pid);
                print_process_info(&info_list[i]);
            }
        } else if (success_count > 0) {
            print_process_info(&info_list[0]);
        }
    }

    return success_count == pid_count ? PSINFO_OK : global_error;
}