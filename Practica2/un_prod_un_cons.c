#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define SIZE_BUFFER 100


typedef struct dato
{
    char caracter;
    int id_hilo;

} DATO_BUFFER;

void* productor(void* args);

void* consumidor(void* args);

DATO_BUFFER* crear_dato(FILE *fp, int id);


/* Variables globales */

DATO_BUFFER buffer[SIZE_BUFFER];

sem_t hay_espacio;
sem_t hay_dato;

int validos = -1;
int no_validos = 0;

int main()
{
    pthread_t hilo_productor;
    pthread_t hilo_consumidor;
    int id_hilo_productor = 1;
    sem_init(&hay_espacio, 0, SIZE_BUFFER);
    sem_init(&hay_dato, 0, 0);

    pthread_create(&hilo_productor, NULL, productor, (void*) &id_hilo_productor);
    pthread_create(&hilo_consumidor, NULL, consumidor, NULL);

    pthread_join(hilo_consumidor, NULL);
    pthread_join(hilo_productor, NULL);

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


void* consumidor(void* args)
{
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