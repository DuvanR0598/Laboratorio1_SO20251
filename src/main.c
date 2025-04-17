#include "process_info.h"
#include <stdio.h>    // Para fprintf
#include <stdlib.h>   // Para atoi
#include <string.h>

#define MAX_PIDS 20  // Máximo número de PIDs que podemos procesar para evitar desbordamiento

int main(int argc, char *argv[]) {

    //Validacion de argumentos 
    if (argc < 2) {
        fprintf(stderr, "Uso: %s [-l] <PID1> <PID2> ...\n", argv[0]);
        return 1;
    }

    int use_list_mode = 0;  // Flag para detectar si usamos el modo -l
    int pid_count = 0;      // Contador de PIDs proporcionados
    pid_t pids[MAX_PIDS];   // Array para almacenar PIDs a procesar 

    // Procesar argumentos
    if (strcmp(argv[1], "-l") == 0) {  //Verificamos si el primer argumento es -l
        use_list_mode = 1;
        if (argc < 3) {
            fprintf(stderr, "Error: Se requiere al menos un PID con -l\n");
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

    // Mostrar resultados según el modo
    if (use_list_mode) {
        printf("\n-- Información recolectada!!!\n");
        for (int i = 0; i < success_count; i++) {
            printf("\nPid: %d\n", info_list[i].pid);
            print_process_info(&info_list[i]);
        }
    } else if (success_count > 0) {
        // Modo simple (mostrar solo un proceso)
        print_process_info(&info_list[0]);
    }

    return success_count == pid_count ? 0 : 1;  // Retorna 0 solo si todos los PIDs fueron exitosos
}