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
    char** args;
    int contador_args = 0;
    int sigue = 1;
    int indice_buffer = 0;
    pid_t pid;

    if(argc > 1)
    {
        fprintf(stderr, "Uso: shell [sin argumentos]\n");
        return -1;
    }
    while(1)
    {
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
                buffer_entrada[indice_buffer] = caracter;
                indice_buffer++;
            }
        }
        buffer_entrada[indice_buffer] = '\0';
        // El buffer de entrada ya contiene el string necesario
	
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
            strcpy(args[contador_args], token);
            contador_args++;
            token = strtok(NULL, DELIMITADOR);
        }
        args[contador_args++] = NULL;
        free(buffer_entrada);
        if(args[0]) 
        {
            if(!strcmp(args[0], "copia")) 
            {
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
                }
            }
            else if (!strcmp(args[0], "lista"))
            {
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
        contador_args = 0;   
    }
}
