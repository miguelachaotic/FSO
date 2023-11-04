#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_CMD_LENGTH 256
#define PROMPT "chiquishell~~> "
#define MAX_ARGS_COUNT 20
#define DELIMITADOR " \n"


char* crear_puntero(int tam);

void print_args(char **args, int num_args);
int ejecuta_hijo(char **args);


int main(int argc, char** argv)
{

    char* buffer_entrada;
    char caracter;
    char* token;
    char** args = (char **)malloc(MAX_ARGS_COUNT * sizeof(char*));
    int contador_args = 0;

    const char* ruta_fichero;
    int sigue = 1;
    int indice_buffer = 0;

    if(argc > 1)
    {
        fprintf(stderr, "Uso: shell [sin argumentos]\n");
    }
    while(1)
    {
        buffer_entrada = crear_puntero(MAX_CMD_LENGTH);
        
        if(!write(1, PROMPT, strlen(PROMPT))) {
            fprintf(stderr, "Error en la escritura del prompt.\n");
            return -1;
        }
        
        while(sigue) {    // Bucle encargado de leer la entrada.
            if(read(0, &caracter, sizeof(char))) {
                if(caracter == '\n') { 
                    sigue = 0; 
                }
                
                buffer_entrada[indice_buffer] = caracter;
                indice_buffer++;
            }
        }
        buffer_entrada[indice_buffer] = '\0';
        // El buffer de entrada ya contiene el string necesario
        
        
        if (buffer_entrada[0] != '\n') {
            token = strtok(buffer_entrada, DELIMITADOR);
        } else { 
          token = NULL; 
        }

        // [!] TODO: No termino de enterarme de que hace cada cosa aquí
        while (token != NULL) {
            args[contador_args] = crear_puntero(strlen(token) + 1);
            strcpy(args[contador_args], token);
            contador_args++;
            token = strtok(NULL, DELIMITADOR);
        }
        args[contador_args++] = NULL;
        
        if(args[0]) 
        {
            if(!strcmp(args[0], "copia")) 
            {
                if(contador_args - 2 != 2) {
                    printf("Warning: numero incorrecto de argumentos para copia.\n");
                }

                strcpy(args[0], "./copia");

                if (ejecuta_hijo(args)){
                    printf("ERROR: durante la ejecución\n");
                    fflush(stdout);
                }
            }

            else if (!strcmp(args[0], "lista"))
            {
                if(contador_args - 2 > 1) {
                    printf("Warning: numero incorrecto de argumentos.\n");
                }

                strcpy(args[0], "/bin/ls");
                if (ejecuta_hijo(args)){
                    printf("ERROR: durante la ejecución\n");
                    fflush(stdout);
                }
            }
            else if (!strcmp(args[0], "muestra")) 
            {
                if(contador_args - 2 > 1) {
                    printf("Warning: numero incorrecto de argumentos.\n");
                }

                strcpy(args[0], "/bin/cat");
                if (ejecuta_hijo(args)) {
                    printf("ERROR: durante la ejecución\n");
                    fflush(stdout);
                }
            }
            else if (!strcmp(args[0], "salir")) {
                for(int i = 0; i < contador_args; i++){
                    free(args[i]);
                }
                free(args);
                free(buffer_entrada);
                return 0;
            }
            else {
                printf("Comando desconocido.\n");
            }
            
            
        }

        sigue = 1;
        indice_buffer = 0;
        contador_args = 0;
    }

}

char* crear_puntero(int tam)
{
    if(tam > MAX_CMD_LENGTH) 
    {
        return NULL;
    }
    return (char*)malloc(tam * sizeof(char));
}

void print_args(char** args, int num_args)
{
    for(int i = 0; i < num_args; i++) 
    {
        printf("Argumento %d: %s\n", i, args[i]);
    }
}

int ejecuta_hijo (char** args) {
    int estado_hijo = 0;
    pid_t pid;

    pid = fork();               // Creamos un hijo
    if (pid < 0) {
        fprintf(stderr, "Error al crear el proceso hijo.\n");
        return -1;
    }

    else if(pid == 0) {       // Código a ejecutar por el hijo
        estado_hijo = execvp(args[0], args);
        if(estado_hijo != 0) {
            printf("Error al buscar %s", args[0]);
            return -1;
        }
    }

    else {
        wait(NULL);
    }

    return 0;
}
