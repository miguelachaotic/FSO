#include <stdio.h>
#include <unistd.h>

#define NH 5

int main(int argc, char* argv[]) {
	int pid[NH];
	for (int i = 0; i < NH; i++){
		printf("ejecutando por %d vez\n", i);
		pid[i] = fork();
		if (pid[i]==0){
			printf("\tNúmero de hijo = %d\n", pid[i]);
		}else{
			printf("Proceso padre\n");
		}
	}
return 0;
}
