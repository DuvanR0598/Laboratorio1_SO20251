#include "process_info.h"
#include <stdio.h>    // Para fprintf
#include <stdlib.h>   // Para atoi
#include <string.h>

//#define MAX_PIDS 20  // Máximo número de PIDs que podemos procesar para evitar desbordamiento

int main(int argc, char *argv[]) {

    //Validacion de argumentos 
    if (argc < 2) {
        fprintf(stderr, "Uso: %s [-l|-r] <PID1> <PID2> ...\n", argv[0]);
        return 1;
    }

    int use_list_mode = 0;    // Flag para detectar si usamos el modo -l
    int use_report_mode = 0;  //Flag para detectar opcion -r
    int pid_count = 0;        // Contador de PIDs proporcionados
    pid_t pids[MAX_PIDS];     // Array para almacenar PIDs a procesar 

    // Procesar argumentos
    if (strcmp(argv[1], "-l") == 0) {  //Verificamos si el primer argumento es -l
        use_list_mode = 1;
    }else if (strcmp(argv[1], "-r") == 0){
        use_report_mode = 1;
    }
    
    if(use_list_mode || use_report_mode) {
        if (argc < 3) {
            fprintf(stderr, "Error: Se requiere al menos un PID con la opción %s\n", argv[1]);
            return 1;
        }

        // Almacenar todos los PIDs después de -l
        for (int i = 2; i < argc && pid_count < MAX_PIDS; i++) {
            pids[pid_count++] = atoi(argv[i]);
        }
    } else {
        // Modo simple (un solo PID)
        pids[pid_count++] = atoi(argv[1]);
    }

    struct process_info info_list[MAX_PIDS];  // Almacenar info de todos los procesos
    int success_count = 0;                    // Para llevar el registro de PIDs procesados correctamente 

    // Obtener información para cada PID
    for (int i = 0; i < pid_count; i++) {
        if (get_process_info(pids[i], &info_list[success_count])) {
            fprintf(stderr, "Error: No se pudo obtener información para PID %d\n", pids[i]);
        } else {
            success_count++;
        }
    }

    // Generar reporte en archivo si se usó -r
    if (use_report_mode && success_count > 0) {
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

    return success_count == pid_count ? 0 : 1;
}