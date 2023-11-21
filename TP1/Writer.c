/*

Curso de Especializacion en Sistemas Embebidos (CESE2023) FIUBA CO20

Sistemas Operativos de Proposito General

Trabajo Practico 1

Estanislao Crivos Gandini

Writer.c

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
#include <signal.h>

/* Defines section */
#define FIFO_NAME 	"FIFO"
#define MSG_DATA 	"DATA:"
#define MSG1 		"SIGN:1"
#define MSG2 		"SIGN:2"
#define MESSAGE_SIZE 	6
#define BUFFER_SIZE 100

/* Variables globales */
char output_buffer[BUFFER_SIZE]; /* Tamano del buffer de escritura */
uint32_t bytes_wrote; /* Bytes escritos */
int32_t return_value; /* Valor de retorno de errores */
int32_t file_descriptor; /* Descriptor de la cola FIFO */

/* Funcion para recibir signals */
void signals_receiver(int signal)
{
    write(1, "Capturo las señales... \n", 23);
   
    if (signal == SIGUSR1)
    {
        strcpy(output_buffer, MSG1);
    }

    if (signal == SIGUSR2)
    {
       strcpy(output_buffer, MSG2);
    }

    /* Escribe en el buffer de la cola FIFO */  
    if ((bytes_wrote = write(file_descriptor, output_buffer, MESSAGE_SIZE)) == -1)
    {
    	perror("write");
    }
    else
    {
        printf("Writer: escribí %d bytes\n", bytes_wrote);
    }
}

/* Programa principal */
int main(void)
{
    char temp_buffer[BUFFER_SIZE];    
    
    /* Crea la cola FIFO */
    if ( (return_value = mknod(FIFO_NAME, S_IFIFO | 0666, 0) ) < -1 )
    {
        printf("Error al crear la cola: %d\n", return_value);
        exit(1);
    }

    /* Abro la cola en modo lectura */
	printf("Esperando lectores...\n");

	/* Si no hay lectores se queda bloqueado */
	if ( (file_descriptor = open(FIFO_NAME, O_WRONLY) ) < 0 )
    {
        printf("Error al abrir al archivo de la cola: %d\n", file_descriptor);
        exit(1);
    }
    
    printf("Existe un lector. Se puede ingresar texto... \n");

    /* Configuracion de las signals */
    struct sigaction sa;
    sa.sa_handler = signals_receiver;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    if(sigaction(SIGUSR1,&sa,NULL) == -1)
    {
        perror("Error de sigaction: SIGUSR1");
        exit(1);
    }
    if(sigaction(SIGUSR2,&sa,NULL) == -1)
    {
        perror("Error de sigaction: SIGUSR2");
        exit(1);
    }

    /* Loop */
	while (1)
	{
        /* Espera texto desde la consola */
		fgets(output_buffer, BUFFER_SIZE, stdin);

        /* Si hay texto... */
        if(strlen(output_buffer) > 1 ) 
        {                  
            strcpy(temp_buffer,MSG_DATA);
            strcat(temp_buffer,output_buffer); 

            /* Escribo en el buffer de la cola */
            if ((bytes_wrote = write(file_descriptor, temp_buffer, strlen(temp_buffer)-1)) == -1)
            {
                perror("write");
            }
            else
            {
                printf("Proceso Writer: escribí %d bytes\n", bytes_wrote);
            }
        }
	}
	return 0;
}
