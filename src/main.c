#include "process_info.h"
#include <stdio.h>    // Para fprintf
#include <stdlib.h>   // Para atoi

int main(int argc, char *argv[]) {

    //Validacion de argumentos 
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <PID>\n", argv[0]);
        return 1;
    }

    pid_t pid = atoi(argv[1]);   // Convierte argumento a número
    struct process_info info;

    // Intenta obtener información del proceso
    if (get_process_info(pid, &info)) {
        fprintf(stderr, "Error: No se pudo obtener información para el PID %d\n", pid);
        return 1;
    }

    print_process_info(&info);  // Muestra los resultados
    return 0;
}