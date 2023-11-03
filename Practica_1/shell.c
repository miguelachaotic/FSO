#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_CMD_LENGTH 256
#define PROMPT "<~~chiquishell~~> "
#define MAX_ARGS_COUNT 20


char* crear_puntero(int tam);

void eliminar_puntero(char* puntero);

void print_args(char** args, int num_args);






int main(int argc, char** argv)
{
    char* buffer_entrada;
    const char* delimitador = " \n";
    char caracter;
    char* token;
    char** args = (char **)malloc(MAX_ARGS_COUNT * sizeof(char*));
    int contador_args = 0;
    pid_t pid;
    const char* ruta_fichero;
    int sigue = 1;
    int estado_hijo = 0;
    int indice_buffer = 0;

    if(argc > 1)
    {
        fprintf(stderr, "Uso: shell [sin argumentos]\n");
        return -1;
    }
    while(1)
    {
        buffer_entrada = crear_puntero(MAX_CMD_LENGTH);
        
        if(!write(1, PROMPT, strlen(PROMPT)))
        {
            fprintf(stderr, "Error en la escritura del prompt.\n");
            return -1;
        }

        while(sigue) // Este bucle lee todo el string de la entrada.
        {
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

        // En este punto el buffer de entrada ya está con el string que necesitamos

        if(buffer_entrada[0] != '\n')
        {
            token = strtok(buffer_entrada, delimitador);
        }
        else { token = NULL; }

        while (token != NULL)
        {
            args[contador_args] = crear_puntero(strlen(token) + 1);
            strcpy(args[contador_args], token);
            contador_args++;
            token = strtok(NULL, delimitador);
        }
        args[contador_args++] = NULL;
        
        //print_args(args, contador_args);
        
        if(args[0])
        {
            if(!strcmp(args[0], "copia")) // Devuelve 0 si son iguales
            {
                if(contador_args - 2 != 2)
                {
                    printf("Warning: numero incorrecto de argumentos para copia.\n");
                }
                pid = fork();
                if(pid < 0)
                {
                    fprintf(stderr, "Error al crear el proceso hijo.\n");
                    return -1;
                }
                else if(pid == 0) // Ejecuta el hijo
                {
                    ruta_fichero = "./copia";
                    strcpy(args[0], ruta_fichero);
                    estado_hijo = execvp(ruta_fichero, args);
                    if(estado_hijo)
                    {
                        fprintf(stderr, "Ha habido un error de ejecucion en el hijo\n");
                        return -1;
                    }
                }
                else
                {
                    wait(NULL);
                }
            }
            else if (!strcmp(args[0], "lista"))
            {
                if(contador_args - 2 > 1)
                {
                    printf("Warning: numero incorrecto de argumentos.\n");
                }
                pid = fork();
                if(pid < 0)
                {
                    fprintf(stderr, "Error al crear el proceso hijo.\n");
                    return -1;
                }
                else if (pid == 0)
                {
                    ruta_fichero = "/bin/ls";
                    strcpy(args[0], ruta_fichero);
                    estado_hijo = execvp(ruta_fichero, args);
                    if(estado_hijo)
                    {
                        fprintf(stderr, "Ha habido un error de ejecucion en el hijo\n");
                        return -1;
                    }
                }
                else
                {
                    wait(NULL);
                }
                
            }
            else if (!strcmp(args[0], "muestra"))
            {
                if(contador_args - 2 > 1)
                {
                    printf("Warning: has introducido 2 o mas parametros.\n");
                }
                pid = fork();
                if(pid < 0)
                {
                    fprintf(stderr, "Error al crear el proceso hijo.\n");
                    return -1;
                }
                else if (pid == 0)
                {
                    ruta_fichero = "/bin/cat";
                    strcpy(args[0], ruta_fichero);
                    estado_hijo = execvp(ruta_fichero, args);
                    if(estado_hijo)
                    {
                        fprintf(stderr, "Ha habido un error de ejecucion en el hijo\n");
                        return -1;
                    }
                }
                else
                {
                    wait(NULL);
                }
                
                
            }
            else if (!strcmp(args[0], "salir")) 
            {
                for(int i = 0; i < contador_args; i++){
                    free(args[i]);
                }
                free(args);
                eliminar_puntero(buffer_entrada);
                return 0;
            }
            else
            {
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

void eliminar_puntero(char* puntero)
{
    free(puntero);
}

void print_args(char** args, int num_args)
{
    for(int i = 0; i < num_args; i++)
    {
        printf("Argumento %d: %s\n", i, args[i]);
    }
}
