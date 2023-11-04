#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_CMD_LENGTH 256
<<<<<<< HEAD
#define PROMPT "chiquishell~~> "
#define MAX_ARGS_COUNT 20
#define DELIMITADOR " \n"
=======
#define PROMPT "<~~chiquishell~~> "
#define MAX_ARGS_COUNT 20
>>>>>>> f5c14e8b529f645e6ec35c2521362ba1afc6b913


char* crear_puntero(int tam);

<<<<<<< HEAD
void print_args(char** args, int num_args);


int main(int argc, char** argv)
{

    char* buffer_entrada;
=======
void eliminar_puntero(char* puntero);

void print_args(char** args, int num_args);






int main(int argc, char** argv)
{
    char* buffer_entrada;
    const char* delimitador = " \n";
>>>>>>> f5c14e8b529f645e6ec35c2521362ba1afc6b913
    char caracter;
    char* token;
    char** args = (char **)malloc(MAX_ARGS_COUNT * sizeof(char*));
    int contador_args = 0;
<<<<<<< HEAD

    const char* ruta_fichero;
    int sigue = 1;
=======
    pid_t pid;
    const char* ruta_fichero;
    int sigue = 1;
    int estado_hijo = 0;
>>>>>>> f5c14e8b529f645e6ec35c2521362ba1afc6b913
    int indice_buffer = 0;

    if(argc > 1)
    {
        fprintf(stderr, "Uso: shell [sin argumentos]\n");
<<<<<<< HEAD
=======
        return -1;
>>>>>>> f5c14e8b529f645e6ec35c2521362ba1afc6b913
    }
    while(1)
    {
        buffer_entrada = crear_puntero(MAX_CMD_LENGTH);
        
<<<<<<< HEAD
        if(!write(1, PROMPT, strlen(PROMPT))) {
            fprintf(stderr, "Error en la escritura del prompt.\n");
            return -1;
        }
        
        while(sigue) {    // Bucle encargado de leer la entrada.
            if(read(0, &caracter, sizeof(char))) {
                if(caracter == '\n') { 
                    sigue = 0; 
                }
                
=======
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
>>>>>>> f5c14e8b529f645e6ec35c2521362ba1afc6b913
                buffer_entrada[indice_buffer] = caracter;
                indice_buffer++;
            }
        }
        buffer_entrada[indice_buffer] = '\0';
<<<<<<< HEAD
        // El buffer de entrada ya contiene el string necesario
        
        
        if (buffer_entrada[0] != '\n') {
            token = strtok(buffer_entrada, DELIMITADOR);
        } else { 
          token = NULL; 
        }

        // TODO: preguntar que hace cada cosa aquí
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
=======

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
>>>>>>> f5c14e8b529f645e6ec35c2521362ba1afc6b913
                for(int i = 0; i < contador_args; i++){
                    free(args[i]);
                }
                free(args);
<<<<<<< HEAD
                free(buffer_entrada);
                return 0;
            }
            else {
=======
                eliminar_puntero(buffer_entrada);
                return 0;
            }
            else
            {
>>>>>>> f5c14e8b529f645e6ec35c2521362ba1afc6b913
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
<<<<<<< HEAD
    if(tam > MAX_CMD_LENGTH) 
=======
    if(tam > MAX_CMD_LENGTH)
>>>>>>> f5c14e8b529f645e6ec35c2521362ba1afc6b913
    {
        return NULL;
    }
    return (char*)malloc(tam * sizeof(char));
}

<<<<<<< HEAD
void print_args(char** args, int num_args)
{
    for(int i = 0; i < num_args; i++) 
=======
void eliminar_puntero(char* puntero)
{
    free(puntero);
}

void print_args(char** args, int num_args)
{
    for(int i = 0; i < num_args; i++)
>>>>>>> f5c14e8b529f645e6ec35c2521362ba1afc6b913
    {
        printf("Argumento %d: %s\n", i, args[i]);
    }
}
<<<<<<< HEAD

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
=======
>>>>>>> f5c14e8b529f645e6ec35c2521362ba1afc6b913
