#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <ctype.h>



typedef struct dato_buffer
{
    char c;
    int id_hilo;
} DATO_BUFFER;

typedef struct datos_productor
{
    int id_hilo; // identificación del hilo que produce datos
    FILE* fp; //Puntero al archivo de donde saca los datos

} DATO_PRODUCTOR;



int esnum(char* cadena);

void* productor(void* args);

void* consumidor(void* args);

void* facturador(void* args);


DATO_BUFFER* buffer_compartido;

int main(int argc, char** argv)
{
    int tam_buffer, num_consumidores, num_productores, ids_consumidores[1000];
    FILE* fichero_salida;
    FILE** ficheros_entrada;
    char ruta_fichero[256]; // Almacena la ruta (relativa o absoluta) a un fichero.
    pthread_t *hilos_productores, *hilos_consumidores, hilo_facturador;
    DATO_PRODUCTOR *datos_productores;


    // Es un dato temporal, se reutiliza con cada proveedor distinto

    if(argc != 6)
    {
        fprintf(stderr, "Uso incorrecto de la vending. Uso correcto:\n ./vending <path> <outputFile> <T> <P> <C>\n");
        exit(1);
    }
    if(!esnum(argv[3]))
    {
        fprintf(stderr, "El tamaño del buffer no es una cadena válida: %s\n", argv[3]);
        exit(1);
    }
    tam_buffer = atoi(argv[3]);
    if(tam_buffer < 1 || tam_buffer > 5000)
    {
        fprintf(stderr, "Tamaño inválido para el buffer circular.\n");
        exit(1);
    }
    if(!esnum(argv[4])){
        fprintf(stderr, "Cadena es alfanúmerica, no válida. %s\n", argv[4]);
        exit(1);
    }
    num_productores = atoi(argv[4]);
    if(num_productores < 1 || num_productores > 7)
    {
        fprintf(stderr, "Número inválido de proveedores. Has introducido %d\n", num_productores);
        exit(1);
    }
    if(!esnum(argv[5])){
        fprintf(stderr, "La cadena introducida es alfanúmerica, no válida. %s\n", argv[5]);
        exit(1);
    }
    num_consumidores = atoi(argv[5]);
    if(num_consumidores < 1 || num_consumidores > 1000)
    {
        fprintf(stderr, "Número inválido de proveedores. Has introducido %d\n", num_consumidores);
        exit(1);
    }
    // Abrimos el archivo en lectura para comprobar que existe
    if((fichero_salida = fopen(argv[2], "r")) != NULL)
    {
        fclose(fichero_salida);
        if((fichero_salida = fopen(argv[2], "w")) == NULL){
            fprintf(stderr, "Error en la apertura del archivo de salida.\n");
            exit(1);
        }
        fclose(fichero_salida);
        printf("WARNING: EL ARCHIVO SELECCIONADO SE SOBREESCRIBIRÁ.\n");
    }

    if((fichero_salida = fopen(argv[2], "a")) == NULL)
    {
        fprintf(stderr, "Error en la apertura del archivo de salida.\n");
        exit(1);
    }

    ficheros_entrada = (FILE**) malloc(num_productores*sizeof(FILE*)); // Puntero a punteros de archivos
    for(int i = 0; i < num_productores; i++)
    {
        sprintf(ruta_fichero, "%s/proveedor%d.dat",argv[1], i);
        if((ficheros_entrada[i] = fopen(ruta_fichero, "r")) == NULL)
        {
            fprintf(stderr, "Error al abrir el fichero %d\n", i);
            exit(1);
        }
    }
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
    printf("He creado bien el vector de consumidores\n");

    if((datos_productores = (DATO_PRODUCTOR*)malloc(num_productores*sizeof(DATO_PRODUCTOR))) == NULL)
    {
        fprintf(stderr, "Error en la creación del vector de datos de los productores.\n");
        exit(1);
    }

    printf("Numero productores = %d\n", num_productores);
    printf("Número de consumidores = %d\n", num_consumidores);
    // Lanzamos los hilos productores
    for(int i = 0; i < num_productores; i++)
    {
        datos_productores[i].fp = ficheros_entrada[i];
        datos_productores[i].id_hilo = i;
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

    for(int i = 0; i < num_productores; i++)
    {
        if(pthread_join(hilos_productores[i], NULL))
        {
            fprintf(stderr, "Error al esperar al hilo productor número %d\n", i+1);
            exit(1);
        }
        if(fclose(ficheros_entrada[i]))
        {
            fprintf(stderr, "Error al cerrar el fichero proveedor%d.dat\n", i);
            exit(1);
        }
    }


    for(int i = 0; i < num_consumidores; i++)
    {
        if(pthread_join(hilos_consumidores[i], NULL))
        {
            fprintf(stderr, "Error al esperar al hilo consumidor número %d.\n", i+1);
            exit(1);
        }
    }



    if(pthread_join(hilo_facturador, NULL))
    {
        fprintf(stderr, "Error al esperar al hilo facturador.\n");
        exit(1);
    }

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

// Recibe un struct con un id y un puntero a fichero
void* productor(void* args)
{
    DATO_PRODUCTOR dato;
    if(args == NULL)
    {
        fprintf(stderr, "Error en los argumentos de la función.\n");
        exit(1);
    }
    dato = *((DATO_PRODUCTOR*) args);
    // En el último productor siempre escribe un 1
    printf("Productor %d: leyendo archivo con puntero %ld\n", dato.id_hilo,(long) dato.fp);
    pthread_exit(NULL);
}

void* consumidor(void* args)
{
    int i = 0;
    while(i++ < 1000000); // Esto hay que quitarlo, solo simula que hace operaciones
    printf("Consumidor terminado con id %d\n", *(int*) args);
    pthread_exit(NULL);
}

void* facturador(void* args)
{
    printf("Facturador escribe en fichero de salida %ld\n", *(long*) args);
    printf("Facturador terminado.\n");
    pthread_exit(NULL);
}
