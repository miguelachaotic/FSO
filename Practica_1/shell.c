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
<<<<<<< HEAD

void print_args(char **args, int num_args);
=======
>>>>>>> ce9c41752dfb06f62f4aac6e23a7deb0fa63d908
int ejecuta_hijo(char **args);


int main(int argc, char** argv)
{

    char* buffer_entrada;
    char caracter;
    char* token;
    char** args;
    int contador_args = 0;
    int sigue = 1;
    int indice_buffer = 0;
    pid_t pid;

    if(argc > 1)
    {
        fprintf(stderr, "Uso: shell [sin argumentos]\n");
    }
    while(1)
    {
<<<<<<< HEAD
        buffer_entrada = (char*) malloc((MAX_CMD_LENGTH+1)*sizeof(char));
        if(!write(1, PROMPT, strlen(PROMPT))) 
        {
            fprintf(stderr, "Error en la escritura del prompt.\n");
            return -1;
        }
        while(sigue) 
        {    // Bucle encargado de leer la entrada.
            if(read(0, &caracter, sizeof(char))) 
            {
                if(caracter == '\n') 
                { 
                    sigue = 0; 
                }       
=======
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
                
>>>>>>> ce9c41752dfb06f62f4aac6e23a7deb0fa63d908
                buffer_entrada[indice_buffer] = caracter;
                indice_buffer++;
            }
        }
        buffer_entrada[indice_buffer] = '\0';
        // El buffer de entrada ya contiene el string necesario
<<<<<<< HEAD
	
        if (buffer_entrada[0] != '\n') 
        {
            token = strtok(buffer_entrada, DELIMITADOR);
        } 
        else 
        { 
          token = NULL; 
        }
        args = (char**) malloc(MAX_ARGS_COUNT* sizeof(char*));
        while (token != NULL) 
        {
            args[contador_args] = (char*)malloc(strlen(token+1)*sizeof(char));
=======
        
        
        if (buffer_entrada[0] != '\n') {
            token = strtok(buffer_entrada, DELIMITADOR);
        } else { 
          token = NULL; 
        }

        // [!] TODO: No termino de enterarme de que hace cada cosa aquí
        while (token != NULL) {
            args[contador_args] = crear_puntero(strlen(token) + 1);
>>>>>>> ce9c41752dfb06f62f4aac6e23a7deb0fa63d908
            strcpy(args[contador_args], token);
            contador_args++;
            token = strtok(NULL, DELIMITADOR);
        }
        args[contador_args++] = NULL;
<<<<<<< HEAD
        free(buffer_entrada);
=======
        
>>>>>>> ce9c41752dfb06f62f4aac6e23a7deb0fa63d908
        if(args[0]) 
        {
            if(!strcmp(args[0], "copia")) 
            {
<<<<<<< HEAD
                if(contador_args - 2 != 2) 
                {
                    printf("Warning: numero incorrecto de argumentos para copia.\n");
                }
                strcpy(args[0], "copia");
                pid = fork();
                if(pid < 0)
                {
                    fprintf(stderr, "Error al crear el proceso hijo.\n");
                    return -1;
                }
                else if(pid == 0)
                {
                    if(!execvp("./copia", args))
                    {
                        fprintf(stderr, "No se ha encontrado el programa %s\n", args[0]);
                    }
                }
                else
                {
                    wait(NULL);
                    for(int i = 0; i < contador_args; i++)
                    {
                        free(args[i]);
                    }
=======
                if(contador_args - 2 != 2) {
                    printf("Warning: numero incorrecto de argumentos para copia.\n");
                }

                strcpy(args[0], "./copia");

                if (ejecuta_hijo(args)){
                    printf("ERROR: durante la ejecución\n");
                    fflush(stdout);
>>>>>>> ce9c41752dfb06f62f4aac6e23a7deb0fa63d908
                }
            }

            else if (!strcmp(args[0], "lista"))
            {
<<<<<<< HEAD
                if(contador_args - 2 > 1) 
                {
                    printf("Warning: numero incorrecto de argumentos.\n");
                }
                strcpy(args[0], "ls");
                pid = fork();
                if(pid < 0)
                {
                    fprintf(stderr, "Error al crear el proceso hijo.\n");
                    return -1;
                }
                else if(pid == 0)
                {
                    if(!execvp("/bin/ls", args))
                    {
                        fprintf(stderr, "No se ha encontrado el programa %s\n", args[0]);
                    }
                }
                else
                {
                    wait(NULL);
                    for(int i = 0; i < contador_args; i++)
                    {
                        free(args[i]);
                    }
                }
            }
            else if (!strcmp(args[0], "muestra")) 
            {
                if(contador_args - 2 > 1) 
                {
                    printf("Warning: numero incorrecto de argumentos.\n");
                }
                strcpy(args[0], "cat");
                pid = fork();
                if(pid < 0)
                {
                    fprintf(stderr, "No se ha podido crear el hijo.\n");
                    return -1;
                }
                else if(pid == 0)
                {
                    if(!execvp("/bin/cat", args))
                    {
                        fprintf(stderr, "No se ha encontrado el archivo %s\n", args[0]);
                        return -1;
                    }
                }
                else
                {
                    wait(NULL);
                    for(int i = 0; i < contador_args; i++){
                        free(args[i]);
                    }
                }
            }
            else if (!strcmp(args[0], "salir")) 
            {
                for(int i = 0; i < contador_args; i++)
                {
                    free(args[i]);
                }
                free(args);
                return 0;
            }
            else 
            {
=======
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
>>>>>>> ce9c41752dfb06f62f4aac6e23a7deb0fa63d908
                printf("Comando desconocido.\n");
                for(int i = 0; i < contador_args; i++)
                {
                    free(args[i]);
                }
            }
        }
        free(args);
        sigue = 1;
        indice_buffer = 0;
<<<<<<< HEAD
        contador_args = 0;   
=======
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

int ejecuta_hijo (char** args) {
    int estado_hijo = 0;
    pid_t pid;

    pid = fork();               // Creamos un hijo
    if (pid < 0) {
        fprintf(stderr, "Error al crear el proceso hijo.\n");
        return -1;
>>>>>>> ce9c41752dfb06f62f4aac6e23a7deb0fa63d908
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
