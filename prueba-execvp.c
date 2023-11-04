#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


int main () {

  char *lista_argumentos[] = {"./copia", "shell.c", "out", NULL};
  int estado;
  printf("\tCreamos proceso con fork()...\n");
  if (fork() == 0) {
    estado = execvp(lista_argumentos[0], lista_argumentos);
    printf("estado = %d", estado);

    printf("ls -l toma control de este hijo. Esto no se debería ejecutar!\n");

    if (estado == -1) { 
      printf("El proceso no termino correctamente \n");
      exit (1);
    }
  }
  else {
    printf("ejecutado por el proceso padre\n");
    wait(NULL);
  }
  return EXIT_SUCCESS;
}

