#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <semaphore.h>

#define SIZE_BUFFER 100

typedef struct dato
{
    char caracter;
    int id_hilo;

} DATO_BUFFER;

int es_num(char* string);
void* productor(void* args);
void* consumidor(void* args);
DATO_BUFFER* crear_dato(FILE *fp, int id);

/* Variables globales */

sem_t hay_espacio, hay_dato;
int validos, no_validos;
DATO_BUFFER* buffer;

int main(int argc, char** argv)
{
    // uso: ./program files output Tam Prov Cons
    int tamaño_buffer, num_proveed, num_consum;
    char* path;
    FILE* archivo_salida;
    FILE** archivos_entrada;

    // Comprobamos cantidad de argumentos
    if (argc != 6) {
        fprintf(stderr, "ERROR: nº de argumentos incorrecto\nUso: ./program input output Tam Prov Cons\n");
        exit (1);
    }
    // Comprobamos que los argumentos sean correctos
    if (!es_num(argv[3])) {
        fprintf(stderr, "ERROR: argumento <tamaño_buffer> no es un número\n");
        exit (1);
    }
    if (!es_num(argv[4])) {
        fprintf(stderr, "ERROR: argumento <num_proveedores> no es un número\n");
        exit (1);
    }
    if (!es_num(argv[5])) {
        fprintf(stderr, "ERROR: argumento <num_consumidores> no es un número\n");
        exit (1);
    }

    tamaño_buffer = atoi(argv[3]);
    num_proveed = atoi(argv[4]);
    num_consum = atoi(argv[5]);
    if (tamaño_buffer < 1 || tamaño_buffer > 5000) {
        fprintf(stderr, "ERROR: argumento <tamaño_buffer> fuera de limites [1,5000]\n");
        exit (1);
    }
    if (num_proveed < 1 || num_proveed > 7) {
        fprintf(stderr, "ERROR: argumento <num_proveedores> fuera de limites [1,7]\n");
        exit (1);
    }
    if (num_consum < 1 || num_consum > 1000) {
        fprintf(stderr, "ERROR: argumento <num_consum> fuera de limites [1,1000]\n");
        exit (1);
    }

    // Comprobamos archivos (salida y entrada)
    if ((archivo_salida = fopen(argv[2], "w")) != NULL) {
        fclose(archivo_salida);
        printf("WARNING: El archivo de salida se sobreescribirá\n");
    }
    if ((archivo_salida = fopen(argv[2], "a")) == NULL) {
        fprintf(stderr, "Error en apertura del archivo de salida\n");
        exit (1);
    }

    path = (char *)malloc(50);
    archivos_entrada = (FILE **)malloc(sizeof(FILE *));
    for (int i = 0; i < num_proveed; i++) {
        sprintf(path, "%s/proveedor%d.dat%c", argv[1], i, '\0');
        if ((archivos_entrada[i] = fopen(path, "r")) == NULL) {
            fprintf(stderr, "Error abriendo los archivos de entrada (%s)\n", path);
            exit (1);
        }
    }
    return 0;
}


void* productor(void* args) 
{
    int n;
    DATO_BUFFER* dato;
    FILE *fp;
    int i = 0;
    n = *((int*)args);
    if(args == NULL)
    {
        fprintf(stderr, "Error en los argumentos\n");
        pthread_exit(NULL);
    }
    if((fp = fopen("../datos_fso/proveedor9.dat", "r")) == NULL)
    {
        fprintf(stderr, "No se pudo abrir el archivo\n");
        pthread_exit(NULL);
    }
    while(1){
        dato = crear_dato(fp, n);
        
        sem_wait(&hay_espacio); //Espera a que haya un espacio
        
        buffer[i] = *dato;

        sem_post(&hay_dato); //Señala que hay dato
        if(dato->caracter == EOF)
        {
            printf("Termina productor\n");
            pthread_exit(NULL);
        }
        free(dato);
        i = (i+1)%SIZE_BUFFER;
    }
}


void* consumidor(void* args) {
    DATO_BUFFER dato;
    int contador[10];
    int j = 0;
    int sigue = 1;
    if(args != NULL)
    {
        fprintf(stderr, "Error en los argumentos\n");
        pthread_exit(NULL);
    }
    while(sigue)
    {
        sem_wait(&hay_dato); //Espera a que haya un dato

        dato = buffer[j];

        sem_post(&hay_espacio); // Señala que hay espacio
        if(dato.caracter == EOF)
        {
            sigue = 0;
        }
        

        contador[dato.caracter-'a']++;

        j = (j+1)%SIZE_BUFFER;

    }
    printf("Objetos válidos: %d, objetos no válidos: %d, total: %d\n", validos, no_validos, validos+no_validos);
    for(int i = 0; i < 10; i++){
        printf("Objeto tipo \"%c\":, cantidad %d\n", (i + 'a'), contador[i]);
    }

    printf("Termina consumidor\n");
    pthread_exit(NULL);
    
}

DATO_BUFFER* crear_dato(FILE *fp, int id)
{
    DATO_BUFFER *nuevo_dato;
    char chr = 0;
    nuevo_dato = (DATO_BUFFER*)malloc(sizeof(DATO_BUFFER));
    while(chr != EOF)
    {
        chr = fgetc(fp);
        if((chr >= 'a' && chr <= 'j') || chr == EOF){
            nuevo_dato->caracter = chr;
            nuevo_dato->id_hilo = id;
            validos++;
            return nuevo_dato;
        } 
        else 
        {
            no_validos++;
        }
    }
    return NULL;
}

int es_num(char* string)
{
    for (int i = 0; i <= strlen(string); i++) {
        if (!isdigit(string[i]) && string[i] != '\0')
            return (0);
    }
    return 1;
}
