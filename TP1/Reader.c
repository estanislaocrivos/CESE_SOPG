/*

Curso de Especializacion en Sistemas Embebidos (CESE2023) FIUBA CO20

Sistemas Operativos de Proposito General

Trabajo Practico 1

Estanislao Crivos Gandini

Reader.c

*/

/* Includes section */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

/* Defines section */
#define FIFO_NAME "FIFO"
#define BUFFER_SIZE 100

/* Main program */
int main(void)
{

	uint8_t inputBuffer[BUFFER_SIZE]; /* Buffer de lectura */
	int32_t bytes_read; /* Bytes leidos */
	int32_t bytes_wrote; /* Bytes escritos */
	int32_t return_value; /* Codigo de error de retorno */
	int32_t file_descriptor; /* File Descriptors de los archivos creados y FIFO */
	int32_t file_descriptor_log;
	int32_t file_descriptor_sign;
	
    
    /* Crea el archivo Log.txt */
	if ( (file_descriptor_log = open("Log.txt", O_CREAT | O_APPEND | O_WRONLY, 0666) ) == -1  )   
    {
        printf("Error al crear el archivo Log.txt: %d\n", return_value);
        exit(1);
    }

    /* Crea el archivo Sign.txt */
    if ( (file_descriptor_sign = open("Sign.txt", O_CREAT | O_APPEND | O_WRONLY, 0666) ) == -1  )   
    {
        printf("Error al crear el archivo Sign.txt: %d\n", return_value);
        exit(1);
    }
    
    /* Crea la cola nombrada */
    if ( (return_value = mknod(FIFO_NAME, S_IFIFO | 0666, 0) ) < -1  )
    {
        printf("Error al crear la cola nombrada: %d\n", return_value);
        exit(1);
    }
    
    /* Abre la cola FIFO nombrada. Se bloquea hasta recibir procesos */
	printf("Esperando escritores...\n");
	if ( (file_descriptor = open(FIFO_NAME, O_RDONLY) ) < 0 )
    {
        printf("Error al abrir al archivo de la cola nombrada: %d\n", file_descriptor);
        exit(1);
    }

	/* Si recibe un escritor, emite un mensaje */
	printf("Hay un escritor de la cola... \n");

    /* Recibe datos siempre y cuando bytes_read sea >0 */
	do
	{
        /* Lee datos del buffer local, si no llegan datos queda bloqueado esperando */
		if ((bytes_read = read(file_descriptor, inputBuffer, BUFFER_SIZE)) == -1)
        {
			perror("read");
        }
        else
		{
			/* Si es un dato... */
			if(inputBuffer[0]=='D')
			{			
				/* Se agrega el /0 al final de la cadena */			
				inputBuffer[bytes_read] = '\0';					
				fprintf(stdout,"Proceso Reader recibió %d bytes: %s \n", bytes_read, inputBuffer);

				/* Escribe los datos en el archivo Log.txt */
				inputBuffer[bytes_read] = '\n';
				if ((bytes_wrote = write(file_descriptor_log, inputBuffer, bytes_read+1)) == -1)
				{
					perror("write");
				}
				else
				{
					fprintf(stdout,"Log.txt: escribí %d bytes \n", bytes_wrote);
				}
			}

			/* Si es una signal... */			
			if(inputBuffer[0]=='S')
			{			
				/* Se agrega el /0 al final de la cadena */			
				inputBuffer[bytes_read] = '\0';					
				fprintf(stdout,"Reader: leí %d bytes: %s \n", bytes_read, inputBuffer);

				/* Escribe los datos en el archivo Sign.txt */
				inputBuffer[bytes_read] = '\n';
				if ((bytes_wrote = write(file_descriptor_sign, inputBuffer, bytes_read+1)) == -1)
				{
					perror("write");
				}
				else
				{
					fprintf(stdout,"Sign.txt: escribí %d bytes \n", bytes_wrote);
				}
			}
		}
	}
	while (bytes_read > 0);

	/* Cierra los file descriptors */       
	close(file_descriptor_log);
	close(file_descriptor_sign);
	close(file_descriptor);

	return 0;
}
