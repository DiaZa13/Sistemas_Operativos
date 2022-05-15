#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
	/* Variables */
	int read_file;
	int write_file;
	char reading_buffer[35]; /*  Buffer que almacena lo que se lee */

	/* Abrir el archivo */
	read_file = open(argv[1], O_RDONLY); /* Archivo de solo lectura */
	write_file = open(argv[2], O_WRONLY); /* Archivo de solo escritura */
	
	if (read_file == -1 || write_file == -1)
		printf("Ha existido un error al abrir el archivo\n");
	else{
		/* Lectura del archivo */
		read(read_file, reading_buffer,strlen(reading_buffer));
		printf("%s\n", reading_buffer);
		/* Escribir/copiar en otro archivo */
		write(write_file, reading_buffer, strlen(reading_buffer));	
	}
	close(read_file); /* Cerrar el archivo luego de leerlo */	
	close(write_file); /* Cerrar el archivo luego de escribir */
	return(0);
}