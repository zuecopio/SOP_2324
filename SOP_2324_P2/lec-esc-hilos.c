/*-----------------------------------------------------+
 |     L E C - E S C - H I L O S . C                   |
 +-----------------------------------------------------+
 |     Versión     : lec-esc_v1                        | 
 |     Autor       : Marcos Belda Martínez             |
 |     Asignatura  : SOP-GIIROB                        |     
 |     Descripción : Programación con hilos. Problema  |
 |                   de la sección crítica             |
 +-----------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

//-----[ COLORES ]------------------------------------(+)

#define RESET_COLOR   "\e[0m"
#define ESCRITORES    "\e[1;37m\e[46m"
#define LECTORES      "\e[1;37m"
  
//----------------------------------------------------(-)

#define MAX_L  3
#define MAX_E  2 
int dato = 0;

void *lector (void *arg) {

    int id = *((int *)arg);
    while (1) {
       
        // Leer datos
        printf (LECTORES " |  LECTOR  ->  %d | · | dato >>  %3d |" RESET_COLOR "\n", id, dato);
        
        // Retraso aleatorio de hasta 1 segundo (en microsegundos)
        usleep (rand() % 1000000);
    }
}

void *escritor (void *arg) {

    int id = *((int *)arg);
    int aux;
    while (1) {
        
        // Escribir datos
        aux = dato;
        usleep (rand() % 1000000);
        aux++;
        usleep (rand() % 1000000);
        dato = aux;
        printf (ESCRITORES " | ESCRITOR ->  %d | · | dato <<  %3d |" RESET_COLOR "\n", id, dato);

        // Retraso aleatorio de hasta 2 segundos (en microsegundos)
        usleep (rand() % 2000000);
    }
}

int main () {

    //-----[ MENÚ DE INICIO ]-----------------------------(+)
    
    printf ("\n");

    printf (" ============= LEC-ESC-HILOS.C ============ \n");    
    printf (" ========================================== \n");
    printf ("   Programación con hilos. Problema de la   \n");
    printf ("   sección crítica.                         \n");
    printf (" ========================================== \n");
    printf ("   No hay ningún mecanismo para evitar la   \n");
    printf ("   condición de carrera, por lo que esta    \n");
    printf ("   se puede apreciar en el código impreso.  \n");
    printf ("                                            \n");    
    printf ("   Diferentes escritores vuelven a escribir \n");  
    printf ("   el mismo número, ahí es donde se ve que  \n");
    printf ("   no hay un control en el acceso al dato,  \n");
    printf ("   entonces este se está modificando        \n");
    printf ("   simultaneamente por varios escritores.   \n");
    printf (" ========================================== \n");
    printf ("   Teclear Ctrl + C para finalizar la       \n");
    printf ("   ejecución del programa.                  \n");
    printf (" ========================================== \n");
    printf (" Pulsar cualquier tecla para continuar... ");
    getchar ();

    printf ("\n");

    //----------------------------------------------------(-)
    
    // MAX_L lectores y MAX_E escritores
    pthread_t lectores[MAX_L], escritores[MAX_E];
    int argLectores[MAX_L], argEscritores[MAX_E];
    int i;
    pthread_attr_t atrib;

    // Inicializar la semilla del generador de números aleatorios
    srand (time(NULL)); 

    // Inicializar la estructura pthread_attr
    pthread_attr_init (&atrib);

    // Crear lectores
    for (i = 0; i < MAX_L; i++) {
      argLectores[i] = i;
      pthread_create (&lectores[i], &atrib, lector, &argLectores[i]);
    }

    // Crear escritores
    for (i = 0; i < MAX_E; i++) {
      argEscritores[i] = i;
      pthread_create (&escritores[i], &atrib, escritor, &argEscritores[i]);
    }

    // Esperar a que los hilos terminen
    for (i = 0; i < MAX_L; i++) {
      pthread_join (lectores[i], NULL);
    }
    
    for (i = 0; i < MAX_E; i++) {
      pthread_join (escritores[i], NULL);
    }
    
    // Acaba el main
    printf ("Acaba el main\n");
    return 0;
}
