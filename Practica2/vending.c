#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <ctype.h>

#define MAX_PRODUCTORES 7
#define MAX_CONSUMIDORES 1000
#define MAX_BUFFER 5000

#define NO_ERRORS 0
#define ERR_MALLOC 1
#define ERR_INIT_SEM 2
#define ERR_ARGS 3
#define ERR_FILE 4
#define ERR_PRINT_STRING 5
#define ERR_CREATE_THREAD 6
#define ERR_JOIN_THREAD 7
#define NULL_POINTER 8



typedef struct dato_buffer
{
    char c;
    int id_hilo;
} DATO_BUFFER;

typedef struct dato_productor
{
    int id_hilo; // identificación del hilo que produce datos
    char *path;
    FILE *fichero_salida; // Puntero al fichero donde escribe datos

} DATO_PRODUCTOR;

typedef struct dato_lista_enlazada
{
    int caracteres[10];     // Los tipos de datos que se han consumido
    int *ids_proveedor;
    int id_consumidor;
    int total;

    struct dato_lista_enlazada *sig;
} NODO;

typedef struct lista_enlazada
{
    NODO *primero;
    NODO *ultimo;
} LISTA_ENLAZADA;




/* DECLARACIÓN DE MÉTODOS */

int esnum(char* cadena);

void* productor(void* args);

void* consumidor(void* args);

void* facturador(void* args);



DATO_BUFFER crear_dato_buffer(FILE* fp, int id_hilo);

int caracter_valido(char c);

/* SEMÁFOROS */

sem_t hay_dato;

sem_t hay_espacio;

sem_t mutex_indice_productores;

sem_t mutex_indice_consumidores;

sem_t mutex_contador_productores;

sem_t mutex_fichero_salida;

sem_t mutex_lista_enlazada;

sem_t hay_dato_lista_enlazada;

/* RECURSOS COMPARTIDOS */

DATO_BUFFER* buffer_compartido;

int indice_productores = 0;

int indice_consumidores = 0;

int contador_productores;

int num_consumidores;

int tam_buffer;

int num_productores;

LISTA_ENLAZADA *lista_enlazada; //Para hacer pruebas usamos un vector primero

void print_lista_enlazada()
{
    NODO* inicio = lista_enlazada->primero;
    if(inicio == NULL){
        printf("Está vacía\n");
        return;
    }
    int i = 0;
    while(inicio != lista_enlazada->ultimo)
    {
        printf("%d\n", i++);
        inicio = inicio->sig;
    }
}

int main(int argc, char** argv)
{
    int ids_consumidores[1000];
    FILE* fichero_salida;
    FILE ** ficheros_entrada;
    char ruta_fichero[256]; // Almacena la ruta (relativa o absoluta) a un fichero.
    pthread_t *hilos_productores, *hilos_consumidores, hilo_facturador;
    DATO_PRODUCTOR *datos_productores;


    // Es un dato temporal, se reutiliza con cada proveedor distinto

    if(argc != 6)
    {
        fprintf(stderr, "Uso incorrecto de la vending.\nUso correcto:\n ./vending <path> <outputFile> <T> <P> <C>\n");
        exit(ERR_ARGS);
    }
    if(!esnum(argv[3]))
    {
        fprintf(stderr, "El tamaño del buffer no es una cadena válida: %s\n", argv[3]);
        exit(ERR_ARGS);
    }
    tam_buffer = atoi(argv[3]);
    if(tam_buffer < 1 || tam_buffer > MAX_BUFFER)
    {
        fprintf(stderr, "Tamaño inválido para el buffer circular.\n");
        exit(ERR_ARGS);
    }
    if(!esnum(argv[4]))
    {
        fprintf(stderr, "Cadena es alfanúmerica, no válida. %s\n", argv[4]);
        exit(ERR_ARGS);
    }
    num_productores = atoi(argv[4]);
    if(num_productores < 1 || num_productores > MAX_PRODUCTORES)
    {
        fprintf(stderr, "Número inválido de proveedores. Has introducido %d\n", num_productores);
        exit(ERR_ARGS);
    }
    if(!esnum(argv[5]))
    {
        fprintf(stderr, "La cadena introducida es alfanúmerica, no válida. %s\n", argv[5]);
        exit(ERR_ARGS);
    }
    num_consumidores = atoi(argv[5]);
    if(num_consumidores < 1 || num_consumidores > MAX_CONSUMIDORES)
    {
        fprintf(stderr, "Número inválido de proveedores. Has introducido %d\n", num_consumidores);
        exit(ERR_ARGS);
    }
    // Abrimos el archivo en lectura para comprobar que existe
    if((fichero_salida = fopen(argv[2], "w")) != NULL)
    {
        if(fclose(fichero_salida))
        {
            fprintf(stderr, "Error al cerrar el fichero de salida.\n");
            exit(ERR_FILE);
        }
        printf("WARNING: EL ARCHIVO SELECCIONADO SE SOBREESCRIBIRÁ.\n");
    }

    if((fichero_salida = fopen(argv[2], "a")) == NULL)
    {
        fprintf(stderr, "Error en la apertura del archivo de salida.\n");
        exit(ERR_FILE);
    }

    if((ficheros_entrada = (FILE**) malloc(num_productores*sizeof(FILE*))) == NULL)
    {
        fprintf(stderr, "Error al crear el vector de punteros de FILE.\n");
        exit(ERR_MALLOC);
    }
    for(int i = 0; i < num_productores; i++)
    {
        if(sprintf(ruta_fichero, "%s/proveedor%d.dat",argv[1], i) < 0)
        {
            fprintf(stderr, "Error al crear el string de la ruta al archivo número %d.\n", i);
            exit(ERR_PRINT_STRING);
        }
        if((ficheros_entrada[i] = fopen(ruta_fichero, "r")) == NULL)
        {
            fprintf(stderr, "Error al abrir el fichero %d\n", i);
            exit(ERR_FILE);
        }
        if(fclose(ficheros_entrada[i]))
        {
            fprintf(stderr, "Error al cerrar fichero de entrada número %d.\n", i);
            exit(ERR_FILE);
        }
    }
    free(ficheros_entrada);
    // En ficheros_entrada están todos los punteros a FILE (FILE*) que ha pedido el usuario
    // Buffer de tamaño tam_buffer de elementos tipo DATO
    if((buffer_compartido = (DATO_BUFFER*)malloc(tam_buffer*sizeof(DATO_BUFFER))) == NULL)
    {
        fprintf(stderr, "Error en la creación del buffer compartido.\n");
        exit(ERR_MALLOC);
    }
    if((hilos_productores = (pthread_t*)malloc(num_productores*sizeof(pthread_t))) == NULL)
    {
        fprintf(stderr, "Error en la creación del vector de hilos productores.\n");
        exit(ERR_MALLOC);
    }
    if((hilos_consumidores = (pthread_t*)malloc(num_consumidores*sizeof(pthread_t))) == NULL)
    {
        fprintf(stderr, "Error en la creación del vector de hilos consumidores.\n");
        exit(ERR_MALLOC);
    }
    if((datos_productores = (DATO_PRODUCTOR*)malloc(num_productores*sizeof(DATO_PRODUCTOR))) == NULL)
    {
        fprintf(stderr, "Error en la creación del vector de datos de los productores.\n");
        exit(ERR_MALLOC);
    }
    if ((lista_enlazada = (LISTA_ENLAZADA*)malloc(sizeof(LISTA_ENLAZADA))) == NULL)
    {
        fprintf(stderr, "Error al crear la lista enlazada.\n");
        exit(ERR_MALLOC);
    }
    lista_enlazada->primero = NULL;
    lista_enlazada->ultimo = NULL;

    if(sem_init(&hay_espacio, 0, tam_buffer))
    {
        fprintf(stderr, "Error al inicializar el semáforo 'hay_espacio'\n");
        exit(ERR_INIT_SEM);
    }
    if(sem_init(&hay_dato, 0, 0))
    {
        fprintf(stderr, "Error al inicializar el semáforo 'hay_dato'\n");
        exit(ERR_INIT_SEM);
    }
    if(sem_init(&mutex_contador_productores, 0, 1))
    {
        fprintf(stderr, "Error al inicializar el semáforo 'mutex_contador_productores'\n");
        exit(ERR_INIT_SEM);
    }
    if(sem_init(&mutex_indice_productores, 0, 1))
    {
        fprintf(stderr, "Error al inicializar el semáforo 'mutex_indice_productores'\n");
        exit(ERR_INIT_SEM);
    }
    if(sem_init(&mutex_indice_consumidores, 0, 1))
    {
        fprintf(stderr, "Error al inicializar el semáforo 'mutex_indice_consumidores'\n");
        exit(ERR_INIT_SEM);
    }
    if(sem_init(&mutex_fichero_salida, 0, 1))
    {
        fprintf(stderr, "Error al inicializar el semáforo 'mutex_fichero_salida'\n");
        exit(ERR_INIT_SEM);
    }
    if(sem_init(&hay_dato_lista_enlazada, 0, 0))
    {
        fprintf(stderr, "Error al inicializar el semáforo 'hay_dato_lista_enlazada'\n");
        exit(ERR_INIT_SEM);
    }
    if(sem_init(&mutex_lista_enlazada, 0, 1))
    {
        fprintf(stderr, "Error al inicializar el semáforo 'mutex_lista_enlazada'\n");
        exit(ERR_INIT_SEM);
    }
    contador_productores = num_productores;

    // Lanzamos los hilos productores
    for(int i = 0; i < num_productores; i++)
    {
        datos_productores[i].fichero_salida = fichero_salida;
        datos_productores[i].id_hilo = i;
        datos_productores[i].path = argv[1]; //Copiamos el puntero de argv[1] al path de cada struct
        if(pthread_create(&hilos_productores[i], NULL, productor, (void*) &datos_productores[i]))
        {
            fprintf(stderr, "No se ha podido crear el hilo productor número %d.\n", i);
            exit(ERR_CREATE_THREAD);
        }
    }

    // Lanzamos los hilos consumidores

    for(int i = 0; i < num_consumidores; i++)
    {
        ids_consumidores[i] = i;
        if(pthread_create(&hilos_consumidores[i], NULL, consumidor, (void*) &ids_consumidores[i]))
        {
            fprintf(stderr, "Error en la creación del hilo consumidor número %d.\n", i);
            exit(ERR_CREATE_THREAD);
        }
    }


    // Lanzamos hilo facturador
    if(pthread_create(&hilo_facturador, NULL, facturador, (void*)fichero_salida))
    {
        fprintf(stderr, "Error al crear el hilo facturador.\n");
        exit(ERR_CREATE_THREAD);
    }

    // Esperamos a los productores
    for(int i = 0; i < num_productores; i++)
    {
        if(pthread_join(hilos_productores[i], NULL))
        {
            fprintf(stderr, "Error al esperar al hilo productor con id %d\n", i);
            exit(ERR_JOIN_THREAD);
        }
    }
    free(hilos_productores);
    free(datos_productores);


    // Esperamos a los consumidores
    for(int i = 0; i < num_consumidores; i++)
    {
        if(pthread_join(hilos_consumidores[i], NULL))
        {
            fprintf(stderr, "Error al esperar al hilo consumidor con id %d.\n", i);
            exit(ERR_JOIN_THREAD);
        }
    }

    free(hilos_consumidores);
    free(buffer_compartido);
    // Esperamos al facturador
    if(pthread_join(hilo_facturador, NULL))
    {
        fprintf(stderr, "Error al esperar al hilo facturador.\n");
        exit(ERR_JOIN_THREAD);
    }
    free(lista_enlazada);
    sem_destroy(&hay_dato);
    sem_destroy(&hay_espacio);
    sem_destroy(&hay_dato_lista_enlazada);
    sem_destroy(&mutex_lista_enlazada);
    sem_destroy(&mutex_indice_consumidores);
    sem_destroy(&mutex_indice_productores);
    sem_destroy(&mutex_contador_productores);
    sem_destroy(&mutex_fichero_salida);

    // Cerramos el fichero de salida
    if(fclose(fichero_salida))
    {
        fprintf(stderr, "Error al cerrar el fichero de salida.\n");
        exit(ERR_FILE);
    }
    exit(NO_ERRORS);

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
    FILE *fichero_entrada, *fichero_salida;
    int contador_caracteres[10];
    char cadena_imprimible_fichero[256];
    char cadena_tipos_caracter[10][64]; // 10 arrays de carácteres
    char ruta_fichero[256];
    int sigue = 1, validos = 0, invalidos = 0, totales = 0, i;
    for(i = 0; i < 10; i++)
    {
        contador_caracteres[i] = 0;
    }
    if(args == NULL)
    {
        fprintf(stderr, "Error en los argumentos del productor.\n");
        exit(NULL_POINTER);
    }
    dato_args = *((DATO_PRODUCTOR*) args);
    fichero_salida = dato_args.fichero_salida;
    if(sprintf(ruta_fichero, "%s/proveedor%d.dat", dato_args.path, dato_args.id_hilo) < 0)
    {
        fprintf(stderr, "Error al crear el path en el hilo %d.\n", dato_args.id_hilo);
        exit(ERR_PRINT_STRING);
    }
    if((fichero_entrada = fopen(ruta_fichero, "r"))== NULL)
    {
        fprintf(stderr, "Error al abrir el fichero proveedor%d.dat.\n", dato_args.id_hilo);
        exit(ERR_FILE);
    }
    while(sigue)
    {

        nuevo_dato = crear_dato_buffer(fichero_entrada, dato_args.id_hilo);
        if(nuevo_dato.c == EOF)
        {
            sem_wait(&hay_espacio); // Esperamos a que haya espacio en el buffer
            sem_wait(&mutex_indice_productores); // Exclusión mutua en el índice
            buffer_compartido[indice_productores] = nuevo_dato;
            indice_productores = (indice_productores + 1)%tam_buffer;
            sem_post(&mutex_indice_productores);
            sem_post(&hay_dato); // Señalamos que ha entrado un nuevo dato

            sigue = 0;
            sprintf(cadena_imprimible_fichero, "Proveedor %d:\n"
                                               "\tProductos procesados: %d.\n"
                                               "\tProductos inválidos: %d.\n"
                                               "\tProductos válidos: %d.\n",
                    dato_args.id_hilo, totales, invalidos, validos);
            for(i = 0; i < 10; i++)
            {
                sprintf(cadena_tipos_caracter[i], "\t%d de tipo \"%c\"\n", contador_caracteres[i], (char)(i + 'a'));
            }
            // Exclusión mutua en el fichero de salida
            sem_wait(&mutex_fichero_salida);
            if(fwrite(cadena_imprimible_fichero, 256, 1, fichero_salida) == 0)
            {
                fprintf(stderr, "Error al escribir datos en el fichero de salida.\n");
                exit(ERR_FILE);
            }
            for(i = 0; i < 10; i++)
            {
                if(fwrite(cadena_tipos_caracter[i], 64, 1, fichero_salida) == 0)
                {
                    fprintf(stderr, "Error al escribir datos en el fichero de salida.\n");
                    exit(ERR_FILE);
                }
            }
            sem_post(&mutex_fichero_salida);

        }
        else if(caracter_valido(nuevo_dato.c))
        {
            totales++;
            validos++;
            sem_wait(&hay_espacio); // Esperamos a que haya espacio en el buffer
            sem_wait(&mutex_indice_productores); // Exclusión mutua en el índice
            buffer_compartido[indice_productores] = nuevo_dato;
            indice_productores = (indice_productores + 1)%tam_buffer;
            sem_post(&mutex_indice_productores);
            sem_post(&hay_dato); // Señalamos que ha entrado un nuevo dato

            // Incrementamos contador
            contador_caracteres[nuevo_dato.c - 'a']++;
        }
        else
        {
            totales++;
            invalidos++;
        }
    }

    if(fclose(fichero_entrada))
    {
        fprintf(stderr, "Error al cerrar el fichero número %d.\n", dato_args.id_hilo);
        exit(ERR_FILE);
    }
    pthread_exit(NULL);
}

void* consumidor(void* args)
{
    char caracter;
    int proveedor;
    NODO *nodo_consumidor;
    DATO_BUFFER dato_buffer;

    int id_hilo = *(int*) args;

    if ((nodo_consumidor = (NODO*)calloc(1, sizeof(NODO))) == NULL)
    {
        fprintf(stderr, "Error al crear el nodo consumidor.\n");
        exit(ERR_MALLOC);
    }
    if ((nodo_consumidor -> ids_proveedor = (int*) calloc (num_productores, sizeof(int))) == NULL)
    {
        fprintf(stderr, "Error al crear el vector de ids de proveedor.\n");
        exit(ERR_MALLOC);
    }
    nodo_consumidor->id_consumidor = id_hilo;

    while(1)
    {
        sem_wait(&hay_dato);
        sem_wait(&mutex_contador_productores);
        if (!contador_productores)
        {
            // No quedan productores, hay que terminar el hilo
            sem_post(&hay_dato);
            sem_post(&mutex_contador_productores);

            sem_wait(&mutex_indice_consumidores);
            indice_consumidores++;
            sem_post(&mutex_indice_consumidores);

            // Añadimos el nodo a la lista enlazada
            sem_wait(&mutex_lista_enlazada);.
            if (lista_enlazada->primero == NULL) 
            {
                lista_enlazada->primero = nodo_consumidor;
                lista_enlazada->ultimo = nodo_consumidor;
            } 
            else 
            {
                lista_enlazada->ultimo->sig = nodo_consumidor;
                lista_enlazada->ultimo = nodo_consumidor;
            }
            sem_post(&mutex_lista_enlazada);
            sem_post(&hay_dato_lista_enlazada); // Mandamos la señal al facturador de que puede
            // consumir el dato y terminamos el proceso

            pthread_exit(NULL);
        } else {
            // Quedan productores activos

            sem_post(&mutex_contador_productores);
            sem_wait(&mutex_indice_consumidores);
            if ((dato_buffer = buffer_compartido[indice_consumidores%tam_buffer]).c == EOF)
            {
                indice_consumidores++;
                sem_post(&mutex_indice_consumidores);
                sem_post(&hay_espacio);
                sem_wait(&mutex_contador_productores);
                contador_productores--;

                // Para solucionar caso en que productor sale, se manda señal
                // hay_espacio pero no se recoge, y consumidor espera que haya espacio
                if (!contador_productores) { sem_post(&hay_dato); }

                sem_post(&mutex_contador_productores);
            } else {
                indice_consumidores++;
                sem_post(&mutex_indice_consumidores);

                caracter = dato_buffer.c;
                proveedor = dato_buffer.id_hilo;

                // Actualizamos los datos en el futuro nodo de la lista enlazada
                nodo_consumidor->caracteres[caracter-'a']++;
                nodo_consumidor->ids_proveedor[proveedor]++;
                nodo_consumidor->total++;

                sem_post(&hay_espacio);
            }
        }
    }
}

void* facturador(void* args)
{

    NODO *dato_lista_enlazada;
    int sigue = 1, contador_consumidores_leidos = 0, total = 0;
    char *texto_fichero;
    FILE *fichero_salida;
    int max_consumidor, max_consumido = 0;
    int *contador_total_proveedores;
    fichero_salida = (FILE*) args;


    if((texto_fichero = (char*) malloc(512 *sizeof(char))) == NULL)
    {
        fprintf(stderr, "Error al reservar memoria.\n");
        exit(ERR_MALLOC);
    }
    if((contador_total_proveedores = (int*) calloc(num_productores, sizeof(int))) == NULL)
    {
        fprintf(stderr, "Error al reservar memoria.\n");
        exit(ERR_MALLOC);
    }
    while(sigue)
    {
        // Vaciamos la cadena
        for (int i = 0; i < 512; ++i)
        {
            texto_fichero[i] = '\0';
        }
        sem_wait(&hay_dato_lista_enlazada);
        sem_wait(&mutex_lista_enlazada);
        dato_lista_enlazada = lista_enlazada->primero;
        sem_post(&mutex_lista_enlazada);
        total += dato_lista_enlazada->total;
        if(max_consumido < dato_lista_enlazada->total)
        {
            max_consumidor = dato_lista_enlazada->id_consumidor;
            max_consumido = dato_lista_enlazada->total;
        }
        for(int i = 0; i < num_productores; i++)
        {
            contador_total_proveedores[i] += dato_lista_enlazada->ids_proveedor[i];
        }
        sprintf(texto_fichero, "Cliente consumidor %d:\n", dato_lista_enlazada->id_consumidor);
        sprintf(texto_fichero, "%sProductos consumidos: %d. De los cuales:\n", texto_fichero, dato_lista_enlazada->total);
        for(int i = 0; i < 10; i++)
        {
            sprintf(texto_fichero, "%s\tProducto tipo \"%c\": %d\n", texto_fichero, i + 'a', dato_lista_enlazada->caracteres[i]);
        }
        if(fwrite(texto_fichero, 512, 1, fichero_salida) == 0)
        {
            fprintf(stderr, "Error al escribir en el fichero.\n");
            exit(ERR_FILE);
        }


        if(contador_consumidores_leidos != num_consumidores)
        {
            if(++contador_consumidores_leidos == num_consumidores)
            {
                sigue = 0;
            }
        }

        sem_wait(&mutex_lista_enlazada);
        lista_enlazada->primero = dato_lista_enlazada->sig;
        free(dato_lista_enlazada);
        sem_post(&mutex_lista_enlazada);
    }
    // Vaciamos la cadena
    for (int i = 0; i < 512; ++i)
    {
        texto_fichero[i] = '\0';
    }
    sprintf(texto_fichero, "\nTotal de productos consumidos: %d.\n", total);
    for(int i = 0; i < num_productores; i++)
    {
        sprintf(texto_fichero, "%s\t%d del proveedor %d.\n", texto_fichero, contador_total_proveedores[i], i);
    }
    sprintf(texto_fichero, "%sCliente consumidor que más ha consumido: %d\n",texto_fichero, max_consumidor);
    fwrite(texto_fichero, 512, 1, fichero_salida);
    free(texto_fichero);
    free(contador_total_proveedores);
    pthread_exit(NULL);
}

DATO_BUFFER crear_dato_buffer(FILE* fp, int id_hilo)
{
    DATO_BUFFER nuevo_dato;
    char c;
    if (fp == NULL)
    {
        fprintf(stderr, "El puntero a fichero no puede ser NULL.\n");
        exit(NULL_POINTER);
    }
    if((c = (char)fgetc(fp)) != EOF)
    {
        nuevo_dato.c = c;
        nuevo_dato.id_hilo = id_hilo;
    }
    else
    {
        nuevo_dato.c = EOF;
        nuevo_dato.id_hilo = -1; // Nos da igual de qué hilo venga EOF, sabemos que tienen que llegar un total
        // de num_productores EOF's
    }
    return nuevo_dato;

}

int caracter_valido(char c)
{
    return (c >= 'a' && c <= 'j');
}

