/*
	Nombre: Antonio David Villegas Yeguas
	Curso 2018/19 - Grupo B2


*/

#include <sys/types.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>


int main(int argc, char * argv[]){
	
	//comprobamos que el numero de argumentos es correcto
	if(argc < 2){
		printf("ERROR: Numero de argumentos erroneo\n \tUso: %s <nombre_archivo> ... <nombre_archivo> <directorio>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	umask(0);

	int tam_total = 0;	


	//recorremos todos los archivos dados
	for(int i = 1; i < argc - 1; i++){

		//abrimos el direcotorio dado
		DIR * directorio = opendir(argv[argc - 1]);
		struct dirent * elemento_dir;
		int encontrado = 0;

		//dentro del directorio, buscamos el archivo con nombre argv[i]
		while ( (elemento_dir = readdir(directorio)) != NULL && !encontrado){
			if (strcmp(elemento_dir->d_name, "..") != 0 && strcmp (elemento_dir->d_name, ".") != 0){
				char archivo[256];
		
				sprintf(archivo, "./%s", elemento_dir->d_name);

				encontrado = strcmp(archivo, argv[i]) == 0;


			}
		}

		//si lo encuentra
		if (encontrado){
			struct stat atributos;
			//leemos atributos
			stat(argv[i], &atributos);

			//comprobamos que sea archivo regular, que usuario, grupos, y otros puedan leer
			if ( S_ISREG(atributos.st_mode) && (atributos.st_mode & 0444) == 0444 ){
			//                                                      ^^^^ hacemos & (and) con los permisos 0444
			//                                                        que son 000 100 100 100, es decir,
			//                                                        activada la lectura para usuario, grupos, y otros
				
				//si se cumple la condicion
				pid_t pid;

				//creamos el fifo con el que se comunicaran el padre y el hijo
				int fd_fifo;
				mkfifo("/tmp/FIFO", 0666);
	
				//abrimos el fifo
				if ( (fd_fifo = open("/tmp/FIFO",O_CREAT|O_WRONLY,S_IRGRP|S_IWGRP|S_IXGRP)) < 0  ){
					printf("\nERROR: No se puede abrir el archivo FIFO\n");
					exit(EXIT_FAILURE);
				}

				//hacemos fork para cada uno
				if ( (pid = fork() ) == 0 ){
	
					//cambiamos la salida estandar al fichero FIFO
					dup2(fd_fifo, STDOUT_FILENO);
					//hijo ejecuta InfoArchivo para el archivo argv[i]
					execl( "./InfoArchivo", "InfoArchivo", argv[i], NULL);

				}else{
					//padre

					char tam[50];

					read(fd_fifo, tam, sizeof(tam));
					tam_total = tam_total + atoi(tam);

				}

			}


		}else {
			//si no lo encuentra, avisa y continuamos
			printf("%s no pertenece al directorio dado\n", argv[i] );	
		}
		
		//cerramos el directorio
		closedir(directorio);

	}

	printf("\nLa suma de todos los bytes es: %d\n", tam_total);

	return EXIT_SUCCESS;

}
