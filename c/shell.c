#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define PROMPT "chiquishell~~> "
#define MAX_CMD_LENGTH 256
#define MAX_ARG_LENGTH 64


void vaciar_buffer(char *buffer)
{
    for(int i = 0; i < MAX_CMD_LENGTH; i++)
    {
        buffer[i] = '\0';
    }
}




int main(int argc, char** argv){

    
    char *buffer;
    char *delimitador = " ";
    char *token;
    char **args;
    int contador_args = 0;
    int tam_antes;


    if(argc > 1){
        fprintf(stderr, "USO: shell [sin argumentos]"); 
    }
   // Hasta que no se introduzca "salir" no se saldrá del shell.
    while (1) 
    {   

        printf(PROMPT);
        fflush(NULL);
        buffer = malloc(MAX_CMD_LENGTH*sizeof(char));
        read(0 , buffer, MAX_CMD_LENGTH*sizeof(char));     
        // En este punto ya tenemos en el buffer la cadena obtenida por el usuario.
        //Ahora comprobaremos si corresponde a alguna cadena de comando
        buffer = strtok(buffer, "\n"); // Elimino el \n de la cadena
        
        
        token = strtok(buffer, delimitador); // Cojo el primer argumento
        tam_antes = sizeof(char*);
        
        args = (char**)malloc(tam_antes);
        
        
        while(token != NULL)
        {   
            
            args[contador_args] = token;
            
            contador_args++;

            token = strtok(NULL, delimitador);
        }
       
        if(!strcmp(args[0], "copia"))
        {
            printf("Ejecuta copia");
        } 
        else if (!strcmp(args[0], "muestra"))
        {
            printf("Ejecuta muestra");
        }
        else if(!strcmp(args[0], "lista"))
        {
            printf("Ejecuta lista");
        }
        else if(!strcmp(args[0], "salir"))
        {
            free(buffer);
            free(args);
            return 0; // Termina el programa
        }
        else
        {
            printf("Comando desconocido\n");
        }

        vaciar_buffer(buffer);
        contador_args = 0;
    }
    



}
