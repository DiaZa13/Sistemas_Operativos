/* Basado y adaptado de:
* https://www.qnx.com/index.html
* https://www.geeksforgeeks.org/posix-shared-memory-api/
* https://users.cs.cf.ac.uk/dave/C/node27.html
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/shm.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<signal.h>
#include<unistd.h>
 
#define SHM_NAME "ZARAY"
#define SHM_SIZE 32


int main(int argc, char** argv){

	/* Variables */
	int shm_fd, truncate; /* share memory file descriptor */
	char* map_pointer; /* pointer to share memory object */
	/* sharing and writing data */
	pid_t child;
	int share_pipe[2];
	int n = atoi(argv[1]);
	char* x = argv[2];
	char* process_id = argv[3];
	int b = strlen(x) + 1;

	/* Open/create a share memory segment */
	shm_fd = shm_open(SHM_NAME, O_CREAT | O_EXCL| O_RDWR, 0666);
	/* Evaluating if the shm segment already exists */
	if(shm_fd == -1){
		/* Validating if the error occurr cause the shm segment alredy exists */
		if(errno == EEXIST){
			shm_fd = shm_open(SHM_NAME, O_RDWR, 0); /* Getting the file descriptor */
			printf("%s: Share memory segment already created with file descriptor %d\n", process_id, shm_fd);
		}else{
			perror("shm_open");
			close(shm_fd);
			shm_unlink(SHM_NAME);
			return EXIT_FAILURE;
		}
	}else{
		/* Memory object's size */
		/* It only happens when the shm object is being created */
		truncate = ftruncate(shm_fd, SHM_SIZE); 
		if(truncate == -1){
			perror("ftruncate");
			close(shm_fd);
			shm_unlink(SHM_NAME);
			return EXIT_FAILURE;
		}

		printf("%s: Created new share memory segment\n", process_id);
	}


	/* Mapping the shm segment to a pointer */
	map_pointer = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

	if(map_pointer == MAP_FAILED){
		perror("mmap");
		// Its not necessary to unmap the shm object, automatic happens when the process finish
		close(shm_fd);
		shm_unlink(SHM_NAME);
		return EXIT_FAILURE;
	}else{
		printf("%s: Map pointer created with value: %6.6X\n", process_id, map_pointer);
	}

	/* Pipe to send x to the child process */
	if (pipe(share_pipe) == -1){
		perror("pipe");
		close(shm_fd);
		shm_unlink(SHM_NAME);
		return EXIT_FAILURE;
	}

	/* Fork */
	child = fork();
	if (child < 0){
		perror("fork");
		close(shm_fd);
		shm_unlink(SHM_NAME);
		return EXIT_FAILURE;
	}

	if (child == 0){
		int a = 0;
		close(share_pipe[1]); /* Close the write space after writing */
		char c[2];
		int bytes, i;

		/* Reading the pipe */
		while(1){
			bytes = read(share_pipe[0], c, b); 			
			if (bytes < 0){
				perror("read pipe");
				close(shm_fd);
				shm_unlink(SHM_NAME);
				return EXIT_FAILURE;		
			}
			if(bytes < 1) break;
			a++;		
		}

		/* Check if the shm segment is empty to avoid overwriting */
		while(*map_pointer != '\0'){
			map_pointer += strlen(x);
		}
	
		/* Another way to avoid overwriting */
		//map_pointer += strlen(map_pointer);


		for (i = 0; i < a; i++){
			sprintf(map_pointer, "%s", c);
			map_pointer += strlen(x);
			
		}
		close(share_pipe[0]); /* Close the read space on the pipe */

	}else{
		int i;
		close(share_pipe[0]); /* Close the read space on the pipe */
		for(i = 1; i < SHM_SIZE; i++){
			if ((i % n) == 0){
				if (write(share_pipe[1], x, b) < 0){
					perror("write");
					close(shm_fd);
					shm_unlink(SHM_NAME);
					return EXIT_FAILURE;
				}
			}
		}
		close(share_pipe[1]); /* Close the write space after writing */
		wait(NULL); /* Wait for the child to finish writing */
		/* Read the share memory segment */
		printf("%s: Share memory has: %s\n", process_id, (char*)map_pointer);

		/* Closing the communications */
		close(shm_fd);
		shm_unlink(SHM_NAME);
	}

	return shm_fd;

}
