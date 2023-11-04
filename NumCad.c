#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define NUMARGS 1

bool cadisnum(char *s);

int main(int argc, char **argv) {
    // 1. Comprobar num parametros
    if (argc == NUMARGS+1){
        if (cadisnum(argv[1]))
            printf("El valor del argumento es %d\n", atoi(argv[1]));
        else{
            printf("ERROR el argumento no es una cadena de números\n");
            return -1;
        }
    }
    else {
        printf("Faltan argumentos. Uso, numcad CADENA\n");
        return -1;
    }
    return 0;
    // 2. Funcion(cadena), devuelve true si todo son dígitos
    // 3. atoi(s)
    // 4. imprimir, etc, ...
}

bool cadisnum(char *s){
    for (int i = 0; i < strlen(s); i++) {
        if (!isdigit(s[i]))
            return false;
    }
    return true;
}