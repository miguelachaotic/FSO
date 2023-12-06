#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <ctype.h>

#define MAX_PRODUCTORES 7
#define MAX_CONSUMIDORES 1000
#define MAX_BUFFER 5000


typedef struct dato_buffer
{
    char c;
    int id_hilo;
} DATO_BUFFER;

typedef struct dato_productor
{
    int id_hilo; // identificación del hilo que produce datos
    char *path;
    FILE* fichero_salida; // Puntero al fichero donde escribe datos


} DATO_PRODUCTOR;

typedef struct dato_lista_enlazada
{
    int total;
    int tipos[10]; // Los tipos de datos que se han consumido
    int id_proveedor[7]; // Solo puede haber como mucho 7 productores

} DATO_LISTA_ENLAZADA;



int esnum(char* cadena);

void* productor(void* args);

void* consumidor(void* args);

void* facturador(void* args);

DATO_BUFFER crear_dato_buffer(FILE* fp, int id_hilo);

int caracter_valido(char c);


void escribe_lista_enlazada(DATO_LISTA_ENLAZADA nuevo_dato);

DATO_BUFFER* buffer_compartido;

sem_t hay_dato;

sem_t hay_espacio;

sem_t mutex_indice_productores;

sem_t mutex_indice_consumidores;

sem_t mutex_contador_productores;

sem_t mutex_fichero_salida;

sem_t mutex_indice_lista_enlazada;

int indice_productores = 0;

int indice_consumidores = 0;

int indice_lista_enlazada;

int contador_productores;


int tam_buffer;


DATO_LISTA_ENLAZADA lista_enlazada[1000]; //Para hacer pruebas usamos un vector primero

int main(int argc, char** argv)
{
    int num_consumidores, num_productores, ids_consumidores[1000];
    FILE* fichero_salida;
    FILE ** ficheros_entrada;
    char ruta_fichero[256]; // Almacena la ruta (relativa o absoluta) a un fichero.
    pthread_t *hilos_productores, *hilos_consumidores, hilo_facturador;
    DATO_PRODUCTOR *datos_productores;


    // Es un dato temporal, se reutiliza con cada proveedor distinto

    if(argc != 6)
    {
        fprintf(stderr, "Uso incorrecto de la vending.\nUso correcto:\n ./vending <path> <outputFile> <T> <P> <C>\n");
        exit(1);
    }
    if(!esnum(argv[3]))
    {
        fprintf(stderr, "El tamaño del buffer no es una cadena válida: %s\n", argv[3]);
        exit(1);
    }
    tam_buffer = atoi(argv[3]);
    if(tam_buffer < 1 || tam_buffer > MAX_BUFFER)
    {
        fprintf(stderr, "Tamaño inválido para el buffer circular.\n");
        exit(1);
    }
    if(!esnum(argv[4]))
    {
        fprintf(stderr, "Cadena es alfanúmerica, no válida. %s\n", argv[4]);
        exit(1);
    }
    num_productores = atoi(argv[4]);
    if(num_productores < 1 || num_productores > MAX_PRODUCTORES)
    {
        fprintf(stderr, "Número inválido de proveedores. Has introducido %d\n", num_productores);
        exit(1);
    }
    if(!esnum(argv[5]))
    {
        fprintf(stderr, "La cadena introducida es alfanúmerica, no válida. %s\n", argv[5]);
        exit(1);
    }
    num_consumidores = atoi(argv[5]);
    if(num_consumidores < 1 || num_consumidores > MAX_CONSUMIDORES)
    {
        fprintf(stderr, "Número inválido de proveedores. Has introducido %d\n", num_consumidores);
        exit(1);
    }
    // Abrimos el archivo en lectura para comprobar que existe
    if((fichero_salida = fopen(argv[2], "w+")) != NULL)
    {
        if(fclose(fichero_salida))
        {
            fprintf(stderr, "Error al cerrar el fichero de salida.\n");
            exit(1);
        }
        printf("WARNING: EL ARCHIVO SELECCIONADO SE SOBREESCRIBIRÁ.\n");
    }

    if((fichero_salida = fopen(argv[2], "a")) == NULL)
    {
        fprintf(stderr, "Error en la apertura del archivo de salida.\n");
        exit(1);
    }

    if((ficheros_entrada = (FILE**) malloc(num_productores*sizeof(FILE*))) == NULL)
    {
        fprintf(stderr, "Error al crear el vector de punteros de FILE.\n");
        exit(1);
    }
    for(int i = 0; i < num_productores; i++)
    {
        if(sprintf(ruta_fichero, "%s/proveedor%d.dat",argv[1], i) < 0)
        {
            fprintf(stderr, "Error al crear el string de la ruta al archivo número %d.\n", i);
            exit(1);
        }
        if((ficheros_entrada[i] = fopen(ruta_fichero, "r")) == NULL)
        {
            fprintf(stderr, "Error al abrir el fichero %d\n", i);
            exit(1);
        }
        if(fclose(ficheros_entrada[i]))
        {
            fprintf(stderr, "Error al cerrar fichero de entrada número %d.\n", i);
            exit(1);
        }
    }
    free(ficheros_entrada);
    //En ficheros_entrada están todos los punteros a FILE (FILE*) que ha pedido el usuario

    // Buffer de tamaño tam_buffer de elementos tipo DATO
    if((buffer_compartido = (DATO_BUFFER*)malloc(tam_buffer*sizeof(DATO_BUFFER))) == NULL)
    {
        fprintf(stderr, "Error en la creación del buffer compartido.\n");
        exit(1);
    }

    if((hilos_productores = (pthread_t*)malloc(num_productores*sizeof(pthread_t))) == NULL)
    {
        fprintf(stderr, "Error en la creación del vector de hilos productores.\n");
        exit(1);
    }

    if((hilos_consumidores = (pthread_t*)malloc(num_consumidores*sizeof(pthread_t))) == NULL)
    {
        fprintf(stderr, "Error en la creación del vector de hilos consumidores.\n");
        exit(1);
    }

    if((datos_productores = (DATO_PRODUCTOR*)malloc(num_productores*sizeof(DATO_PRODUCTOR))) == NULL)
    {
        fprintf(stderr, "Error en la creación del vector de datos de los productores.\n");
        exit(1);
    }

    contador_productores = num_productores;
    sem_init(&hay_espacio, 0, tam_buffer);
    sem_init(&hay_dato, 0, 0);
    sem_init(&mutex_contador_productores, 0, num_productores);
    sem_init(&mutex_indice_productores, 0, 1);
    sem_init(&mutex_indice_consumidores, 0, 1);

    // Lanzamos los hilos productores
    for(int i = 0; i < num_productores; i++)
    {
        datos_productores[i].fichero_salida = fichero_salida;
        datos_productores[i].id_hilo = i + 1;
        datos_productores[i].path = argv[1]; //Copiamos el puntero a argv[1]
        printf("Id del hilo: %d\n", datos_productores[i].id_hilo);
        if(pthread_create(&hilos_productores[i], NULL, productor, (void*) &datos_productores[i]))
        {
            fprintf(stderr, "No se ha podido crear el hilo productor número %d.\n", i);
            exit(1);
        }
    }

    // Lanzamos los hilos consumidores

    for(int i = 0; i < num_consumidores; i++)
    {
        ids_consumidores[i] = i + 1;
        if(pthread_create(&hilos_consumidores[i], NULL, consumidor, (void*) &ids_consumidores[i]))
        {
            fprintf(stderr, "Error en la creación del hilo consumidor número %d.\n", i);
            exit(1);
        }
    }


    // Lanzamos hilo facturador
    if(pthread_create(&hilo_facturador, NULL, facturador, (void*)fichero_salida))
    {
        fprintf(stderr, "Error al crear el hilo facturador.\n");
        exit(1);
    }

    // Esperamos a los productores
    for(int i = 0; i < num_productores; i++)
    {
        if(pthread_join(hilos_productores[i], NULL))
        {
            fprintf(stderr, "Error al esperar al hilo productor número %d\n", i+1);
            exit(1);
        }
    }
    free(hilos_productores);
    free(datos_productores);

    // Esperamos a los consumidores
    for(int i = 0; i < num_consumidores; i++)
    {
        if(pthread_join(hilos_consumidores[i], NULL))
        {
            fprintf(stderr, "Error al esperar al hilo consumidor número %d.\n", i+1);
            exit(1);
        }
    }

    free(hilos_consumidores);
    free(buffer_compartido);


    // Esperamos al facturador
    if(pthread_join(hilo_facturador, NULL))
    {
        fprintf(stderr, "Error al esperar al hilo facturador.\n");
        exit(1);
    }

    // Cerramos el fichero de salida
    if(fclose(fichero_salida))
    {
        fprintf(stderr, "Error al cerrar el fichero de salida.\n");
        exit(1);
    }


    exit(0);

}


int esnum(char* cadena)
{
    for(int i = 0; i < strlen(cadena); i++)
    {
        if(!isdigit(cadena[i]) && cadena[i] != '\n')
        {
            return 0;
        }
    }
    return 1;
}

void* productor(void* args)
{
    DATO_PRODUCTOR dato_args;
    DATO_BUFFER nuevo_dato;
    FILE* fp;
    char ruta_fichero[256];
    int sigue = 1;

    if(args == NULL)
    {
        fprintf(stderr, "Error en los argumentos de la función.\n");
        exit(1);
    }
    dato_args = *((DATO_PRODUCTOR*) args);
    if(sprintf(ruta_fichero, "%s/proveedor%d.dat", dato_args.path, dato_args.id_hilo-1) < 0)
    {
        fprintf(stderr, "Error al crear el path en el hilo %d.\n", dato_args.id_hilo);
        exit(1);
    }
    if((fp = fopen(ruta_fichero, "r"))== NULL)
    {
        fprintf(stderr, "Error al abrir el fichero proveedor%d.dat.\n", dato_args.id_hilo-1);
        exit(1);
    }
    while(sigue)
    {
        nuevo_dato = crear_dato_buffer(fp, dato_args.id_hilo);
        sem_wait(&hay_espacio);
        sem_wait(&mutex_indice_productores);
        buffer_compartido[indice_productores] = nuevo_dato;
        sem_post(&mutex_indice_productores);
        sem_post(&hay_dato);
        printf("He escrito el caracter %c en el buffer con id %d.\n", nuevo_dato.c, nuevo_dato.id_hilo);
        if(nuevo_dato.id_hilo != -1)
        {
            indice_productores = (indice_productores + 1)%tam_buffer;
        }
        else
        {
            sigue = 0;
            printf("Productor termina.\n");
        }
    }

    if(fclose(fp))
    {
        fprintf(stderr, "Error al cerrar el fichero número %d.\n", dato_args.id_hilo-1);
        exit(1);
    }
    pthread_exit(NULL);
}

void* consumidor(void* args)
{
    DATO_LISTA_ENLAZADA nuevo_dato;
    DATO_BUFFER dato_buffer;
    int id_hilo = *(int*) args;
    int sigue = 1;
    int contador[10];
    while(sigue)
    {
        sem_wait(&mutex_contador_productores);
        if(contador_productores == 0)
        {
            sem_post(&mutex_contador_productores);
            sigue = 0;
            // Escribir aquí estadísticas en la lista enlazada
            // y eso
            printf("Consumidor termina.\n");
        }
        else
        {
            sem_post(&mutex_contador_productores);
            sem_wait(&hay_dato);
            sem_wait(&mutex_indice_consumidores);
            dato_buffer = buffer_compartido[indice_consumidores];
            sem_post(&mutex_indice_consumidores);
            sem_post(&hay_espacio);
            printf("Consumidor %d lee %c con id: %d\n",id_hilo, dato_buffer.c, dato_buffer.id_hilo);
            if(dato_buffer.c == EOF)
            {
                sem_wait(&mutex_contador_productores);
                contador_productores--;
                sem_post(&mutex_contador_productores);
            }
            sem_wait(&mutex_indice_consumidores);
            indice_consumidores = (indice_consumidores + 1)%tam_buffer;
            sem_post(&mutex_indice_consumidores);
            contador[dato_buffer.c - 'a']++;
        }
    }
    pthread_exit(NULL);
}

void* facturador(void* args)
{
    printf("Facturador escribe en fichero de salida %ld\n", *(long*) args);
    printf("Facturador terminado.\n");
    pthread_exit(NULL);
}

DATO_BUFFER crear_dato_buffer(FILE* fp, int id_hilo)
{
    DATO_BUFFER nuevo_dato;
    char c = 0;
    if (fp == NULL)
    {
        fprintf(stderr, "El puntero a fichero no puede ser NULL.\n");
        exit(1);
    }

    while(1)
    {
        nuevo_dato.id_hilo = -1;
        c = (char)fgetc(fp);
        if(caracter_valido(c)) {
            nuevo_dato.id_hilo = id_hilo;
            nuevo_dato.c = c;
            return nuevo_dato;
        }
        else if(c == EOF)
        {
            nuevo_dato.id_hilo = -1;
            nuevo_dato.c = c;
            return nuevo_dato;
        }


    }
    return nuevo_dato;


}

int caracter_valido(char c)
{
    return (c >= 'a' && c <= 'j') ? 1 : 0;
}

void escribe_lista_enlazada(DATO_LISTA_ENLAZADA nuevo_dato)
{

}

