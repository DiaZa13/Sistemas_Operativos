#include <stdio.h>
#include <unistd.h>

int main(){
	int f = fork();
	if (f == 0)
		execl("ej1a", (char*)NULL);	
	else{
		printf("Hello from Parent ");
		printf("%d\n", (int)getpid());			
		execl("ej1a", (char*)NULL);
	}
	return(0);
}