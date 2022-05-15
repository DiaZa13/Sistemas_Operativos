#include<stdio.h>
#include<unistd.h>

int main(){
	
	int child = fork();
	switch(child) {
 
	case -1:
		perror("fork");
		break;
	case 0:
		execl("ipc", "ipc", (char*)"10", (char*)"a", (char*)"A",NULL);
		break;
	default:
		execl("ipc", "ipc", (char*)"5", (char*)"b", (char*)"B",NULL);
		usleep(600);
	}
	
	return 0;

}
