#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define NUMARGS 1

int main(int argc, char **argv) {
    // 1. Ver número de argumentos
    if (argc != NUMARGS+1){
        printf("ERROR, se toma un parametro\ncat.c FICHERO");
        return -1;
    }
    else{
        FILE *fp;
        // 2. Abrir, existe??
        if (fp = fopen(argv[1], "r")) {
            printf("Fichero %s abierto :D\n", argv[1]);
        }else{
            printf("ERROR, el fichero %s no existe\n", argv[1]);
            return -1;
        }
        char* cadena[254];
        printf("tratando de leer");
        while (fgets(*cadena, 254, fp) != NULL) {
            fp += 1;
            printf("Puntero: %s", *cadena);
        }
        fclose(fp);
    }

    // 3. Modo de lectura? ( )fread, ( )fsanf, ( )fgets, (v)fgetc, ... fgetc poco eficiente pero muy general. Bucles
    // while (char[] cadena = (fgets() != EOF)){
    //     printf("%s", cadena);
}
