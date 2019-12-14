/*
	Nombre: Antonio David Villegas Yeguas
	Curso 2018/19 - Grupo B2


*/

#include <sys/types.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>


int main(int argc, char * argv[]){
	
	//comprobamos que el numero de argumentos es correcto
	if(argc != 2){
		printf("ERROR: Numero de argumentos erroneo\n \tUso: %s <nombre_archivo>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	//leemos la informacion del archivo
	struct stat metadatos;
	if (stat(argv[1], &metadatos) < 0 ){
		printf("ERROR: No se puede acceder a los atributos de %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	//Comprobamos si es un archivo regular
	if( ! S_ISREG(metadatos.st_mode) ){
		printf("ERROR: %s no es un archivo regular \n", argv[1]);
		exit(EXIT_FAILURE);
	}

	umask(0);

	pid_t pid_actual;

	char nombre_completo[50];
	int fd;
	int fd_arg;
	
	//obtenemos el pid del proceso actual
	pid_actual = getpid();
	
	//establecemos el nombre completo como <nombre_arhivo>.<pid>
	sprintf(nombre_completo, "%s.%d", argv[1], pid_actual);


	//creamos el archivo, si existe, lo sobreescribimos	
	if ( (fd = open(nombre_completo,O_CREAT|O_TRUNC|O_WRONLY,S_IRGRP|S_IWGRP|S_IXGRP)) < 0  ){
		printf("\nERROR: No se puede crear el archivo de salida\n");
		exit(EXIT_FAILURE);
	}

	//abrimos el archivo dado por parametro
	if ( (fd_arg = open(argv[1], O_RDONLY,S_IRGRP|S_IWGRP|S_IXGRP)) < 0  ){
		printf("\nERROR: No se puede abrir el archivo pasado por parametro\n");
		exit(EXIT_FAILURE);
	}

	char mensaje[200];
	char primeros_caracteres[50];

	//leemos los primeros 50 bytes del archivo dado
	if (read(fd_arg, primeros_caracteres, 50 ) < 0 ){
		printf("\nERROR: No se ha podido leer la informacion del archivo\n");
		exit(EXIT_FAILURE);
	}

	//establecemos el mensaje como: propietario i-nodo, tamaño, y los primeros 50 caracteres
	sprintf(mensaje, "%d \n %d \n %d \n %s \n", (int)metadatos.st_uid, (int)metadatos.st_ino, (int)metadatos.st_size, primeros_caracteres);

	//los escribimos al archivo de salida
	if ( write(fd, mensaje, sizeof(mensaje) ) < 0 ){
		perror("\nERROR: No se ha podido escribir la informacion del archivo\n");
		exit(EXIT_FAILURE);
	}

	//sacamos por salida estandar el tamaño del archivo
	char tam[50];
	sprintf(tam, "%d", (int)metadatos.st_size);
	
	if (write(1, tam, sizeof(tam)) < 0 ){
		perror("\nERROR: No se ha podido escribir el tamaño por salida estandar\n");

	}


	return 0;

}
