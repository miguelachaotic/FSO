#include <stdio.h>
#include <math.h>

const double PI = 3.141592;

double areaCirculo (double r);
double perimetroCirculo (double r);
double media (int x, int y, int z);

int main () {
	double r;
  int x,y,z;
  printf("Introduce el radio (r) = ");
	scanf("%lf", &r);

	printf("a)\tArea = %lf\n", areaCirculo(r));
  printf("b)\tPerimetro = %lf\n", perimetroCirculo(r));

  printf("\nIntroduzca 3 numeros (x y z) = ");
  scanf("%d %d %d", &x, &y, &z);
  printf("c)\tMedia = %f\n", media(x,y,z));
}


 // #####     FUNCIONES     #####

double areaCirculo (double r){
  return r*r*PI;
}
double perimetroCirculo (double r){
  return 2*PI*r;
}
double media (int x, int y, int z){
  return (x+y+z)/3;
}
