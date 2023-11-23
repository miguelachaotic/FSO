// 1 solución de stackoverflow
// https://stackoverflow.com/questions/40855584/c-producer-consumer-using-pthreads

#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>

#define TAMAÑO_BUFFER 5
#define MAX_PRODUCTOS 50
#define TERMINADOR 50   // Dato terminador

void* productor(void* args);
void* consumidor(void* args);

int buffer[TAMAÑO_BUFFER];
sem_t hay_espacio;
sem_t hay_dato;
sem_t puedo_consumir;       // Producto para evitar la sección crítica entre los consumidores y su contador global
int contador_consum = 0;    // Contador global de los consumidores
int sigue_consum = 1;       // Bandera para terminar con los consumidores (no funciona)

int main(){
    pthread_t thds[6];
    sem_init(&hay_espacio, 0, TAMAÑO_BUFFER);
    sem_init(&hay_dato, 0, 0);
    sem_init(&puedo_consumir, 0, 1);

    pthread_create(&thds[0], NULL, productor,  NULL);

    for (int i = 0; i <= 4; i++) {
        pthread_create(&thds[i], NULL, consumidor, (void*) i);
    }

    pthread_join(thds[0], NULL);
    pthread_join(thds[1], NULL);

    return 0;
}


void* productor(void* args){
    // 1 productor va guardando hasta el número 50 en el buffer
    printf("Productor creado\n");
    for (int i = 0; i <= MAX_PRODUCTOS; i++) {
        sem_wait(&hay_espacio);

        printf("\tguardo %d en pos %d...\n", i, i%TAMAÑO_BUFFER);   
        fflush(stdout);

        buffer[i%TAMAÑO_BUFFER] = i;
        sem_post(&hay_dato);
    }
    pthread_exit(NULL);
}

void* consumidor(void* args){
    int id = (int) args;
    printf("Consumidor %d creado\n", id);

    while (sigue_consum) {
        sem_wait(&hay_dato);
        sem_wait(&puedo_consumir);  // Espera a que otro consumidor que haya entrado a 
                                    // la sección crítica actualice el valor del contador global
                                    // sino, puede acceder a un dato ya consumido y se sumen 2 al contador

        if (buffer[contador_consum%TAMAÑO_BUFFER] == TERMINADOR) {
            printf("PROD FINAL ENCONTRADO (por %d)\n", id);
            sigue_consum = 0;
        }
        printf("cons no. %d, pos %d = %d\n", id, contador_consum%TAMAÑO_BUFFER, buffer[contador_consum%TAMAÑO_BUFFER]);   
        fflush(stdout);
        contador_consum ++; // Aumento el contador global

        sem_post(&hay_espacio);
        sem_post(&puedo_consumir);
    }
    pthread_exit(NULL);
}
