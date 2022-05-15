#include <stdio.h>
#include <unistd.h>

int main(){
	printf("Hello World!\n");
	/* getpid: llamada de sistema que devuelve el numero de proceso */
	printf("%d\n", (int)getpid()); /* id del proceso en el que se ejecuta el programa */
	return(0);
}