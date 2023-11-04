#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXIT_FAILURE 1

int main (int argc, char ** argv)
{
  char* ruta0, * ruta1;
  ruta0 = argv[1];
  ruta1 = argv[2];
  
  FILE * pFich0, * pFich1;
  char a = 0;

  if (argc != 3){
    fprintf(stderr, "USO: copia [archivo_org] [archivo_dest]\n");
    exit (EXIT_FAILURE);
  } 
  else if (strcmp(ruta0, ruta1) == 0) {
    fprintf(stderr, "ERROR: los 2 archivos no pueden ser el mismo\n");
    exit (EXIT_FAILURE);
  }
  else if ((pFich1 = fopen(ruta1, "r")) != NULL) {
    fprintf(stderr, "ERROR: el archivo destino ya existe\n");
    fclose(pFich1);
    exit(EXIT_FAILURE);
  } 
  else if ((pFich0 = fopen(ruta0, "r")) == NULL) {
    fprintf(stderr, "ERROR: el archivo origen no existe\n");
    exit(EXIT_FAILURE);
	}
  
  pFich1 = fopen(ruta1, "w");
  // Si no hay ningún error:

  while ( (a = fgetc(pFich0)) != EOF ) {
    fputc(a, pFich1);
  }

  fclose(pFich0); fclose(pFich1);
  
  return 0;
}
