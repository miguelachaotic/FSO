#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


#define PROMPT "<~~chiquishell~~> "
#define EXIT_ERR 1
#define MAX_CMD_LENGTH 256
#define MAX_ARGS 20



void vaciar_buffer(char *buffer)
{
    for(int i = 0; i < MAX_CMD_LENGTH; i++)
    {
        buffer[i] = '\0';
    }
}

char* crear_buffer()
{
    char* nuevo_buffer;
    nuevo_buffer = (char*)malloc(MAX_CMD_LENGTH*sizeof(char));
    return nuevo_buffer;
}

void eliminar_buffer(char* buffer)
{
    free(buffer);
}

void print_args(char** args, int num_args)
{
    for(int i = 0; i < num_args; i++)
    {
        printf("%s\n", args[i]);
    }
}

void reset_args(char** args)
{
    int i = 0;
    while(args[i] != NULL)
    {
        free(args[i]);
        i++;
    }
}




int main(int argc, char** argv)
{

    char *buffer;
    const char *delimitador = " \n";
    char caracter;
    char *token;
    char *args[MAX_ARGS];
    int contador_args = 0;
    int anterior;
    
    pid_t pid;
    const char *file_path;
    int sigue = 1;
    int estado_hijo = 0;
    int indice_buffer = 0;
    

    if(argc > 1){
        fprintf(stderr, "USO: shell [sin argumentos]\n"); 
        return -1;
    }
    do
    {   
        
        buffer = crear_buffer(); // Crea un nuevo puntero buffer
        vaciar_buffer(buffer);
        if(write(1, PROMPT, strlen(PROMPT)) == -1)
        {
            fprintf(stderr,"Error en la escritura.");
            exit(EXIT_ERR);
        }
        
        do
        {      
            
            if(read(0 , &caracter, 1) != -1 && sigue)
            {
                
                if(caracter == '\n') 
                {
                    sigue = 0;
                }
                buffer[indice_buffer] = caracter;  
            }
            indice_buffer++;
        }
        while (sigue);
        buffer[indice_buffer] = '\0';
        
        
        
        
            
        // En este punto ya tenemos en el buffer la cadena obtenida por el usuario.
        //Ahora comprobaremos si corresponde a alguna cadena de comando
        
        if(buffer[0] != '\n')
        {
            token = strtok(buffer, delimitador); // Cojo el primer argumento
            strcat(token, "\0");
        }
        else { token = NULL; }
        while(token != NULL && contador_args < MAX_ARGS)
        {    
            args[contador_args] = (char*)malloc((strlen(token)+1) * sizeof(char));
            strcpy(args[contador_args], token);
            contador_args++;
            token = strtok(NULL, delimitador);
            strcat(token, "\0");
            
            
        }
        //printf("%s\n", buffer);
        print_args(args, anterior);
        if(args[0] != NULL)
        {
            
            if(!strcmp(args[0], "copia"))
            {
                pid = fork();
                if(pid < 0)
                {
                    fprintf(stderr, "No se ha podido crear el hijo.");
                    return -1;
                }
                else if(pid == 0)
                {
                    file_path = "./copia";
                    estado_hijo = execvp(file_path, args);
                    if(estado_hijo)
                    {
                        fprintf(stderr, "Error en la ejecucion del proceso hijo.\n");
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
                pid = fork();
                if(pid < 0)
                {
                    fprintf(stderr, "Error al crear el proceso hijo.\n");
                    return -1;
                }
                else if(pid == 0)
                {
                    file_path = "/bin/cat";
                    strcpy(args[0], file_path);
                    //print_args(args, contador_args);
                    estado_hijo = execvp(file_path, args);
                    if(estado_hijo)
                    {
                        fprintf(stderr, "Error en la ejecucion del proceso hijo.\n");
                    }
                }
                else
                {
                    wait(NULL);
                }
            }
            else if(!strcmp(args[0], "lista"))
            {
                pid = fork();
                if(pid < 0)
                {
                    fprintf(stderr, "Error al crear el proceso hijo.\n");
                    return -1;
                }
                else if(pid == 0)
                {
                    file_path = "/bin/ls";
                    strcpy(args[0], file_path);
                    estado_hijo = execvp(file_path, args);
                    if(estado_hijo)
                    {
                        fprintf(stderr, "Error en la ejecucion del proceso hijo.\n");
                    }
                }
                else
                {
                    wait(NULL);
                }
            }
            else if(!strcmp(args[0], "salir"))
            {
                reset_args(args);
                free(buffer);
                return 0; // Termina el programa
            }
            else
            {
                printf("Comando desconocido\n");
            }
        }
        
        eliminar_buffer(buffer);
        reset_args(args);
        sigue = 1;
        anterior = contador_args;
        indice_buffer = 0;
        contador_args = 0;
        
    
    }while(1);

}
