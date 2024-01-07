/*-----------------------------------------------------+
 |     L E C - E S C - S E M A F O R O S . C           |
 +-----------------------------------------------------+
 |     Versión     : LemonTree_v1                      | 
 |     Autor       : Marcos Belda Martínez             |
 |     Asignatura  : SOP-GIIROB                        |     
 |     Descripción : Sección crítica: solución con     |
 |                   semáforos POSIX                   |
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
#define INFO_LEC      "\e[1;35m"
#define INFO_ESC      "\e[1;36m"
  
//----------------------------------------------------(-)

#define MAX_L  3
#define MAX_E  2 


int dato = 0;

sem_t mutex;         // Semáforo para proteger las secciones críticas
sem_t m_escritores;  // Semáforo para controlar el acceso de los escritores
int nlectores = 0;   // Variable que indica el número de lectores leyendo

void *lector (void *arg) {

    int id = *((int *)arg);
    while (1) {
       
        // 1) Incremento el número de lectores
        sem_wait (&mutex); // P(mutex)
        
        nlectores++;
        
        // 2) Si soy el primer lector entonces intento entrar a leer
        if (nlectores == 1) { sem_wait (&m_escritores); } // P(m_escritores)
        sem_post (&mutex); // V(mutex)
        
        usleep (1);
        
        // 3) Leer datos
        // Como se ha implemntado un semáforo para los escritores, cuando haya lectores leyendo, el nº de esc. activos siempre será 0
        printf (LECTORES " |  LECTOR  ->  %d | · | dato >>  %3d | ······· " INFO_LEC "| lec. activos = %d | · | esc. activos = 0 |" RESET_COLOR "\n", id, dato, nlectores);
        
        // 4) Decremento el número de lectores
        sem_wait (&mutex); // P(mutex)
        nlectores--;
        
        // 5) Si soy el último, entonces desbloqueo al posible escritor que esté esperando
        if (nlectores == 0) { sem_post (&m_escritores); } // V(m_escritores)
        sem_post (&mutex); // V(mutex)
        
        // 6) Retraso aleatorio de hasta 1 segundo (en microsegundos)
        usleep (rand() % 1000000);
    }
}

void *escritor (void *arg) {

    int id = *((int *)arg);
    int aux;
    while (1) {
	
	// 1) Si no hay lectores entonces intento entrar a escribir
	sem_wait (&m_escritores); // P(m_escritores)
	        
    // 2) Escribir datos
    aux = dato;
    usleep (rand() % 1000000);
    aux++;
    usleep (rand() % 1000000);
    dato = aux;
    // Como se ha implemntado un semáforo para los escritores, cuando no haya lectores leyendo, el nº de esc. activos siempre será 1
    printf (ESCRITORES " | ESCRITOR ->  %d | · | dato <<  %3d |" RESET_COLOR LECTORES " ······· " INFO_ESC "| lec. activos = %d | · | esc. activos = 1 |" RESET_COLOR "\n", id, dato, nlectores-1);
        
    // 3) Acabo de escribir, entonces desbloqueo al posible escritor que esté esperando
	sem_post (&m_escritores); // V(m_escritores)

	// 4) Retraso aleatorio de hasta 2 segundo (en microsegundos)
        usleep (rand() % 2000000);
    }
}

int main () {

    //-----[ MENÚ DE INICIO ]-----------------------------(+)
    
    printf ("\n");

    printf (" =========== LEC-ESC-SEMAFOROS.C ========== \n");    
    printf (" ========================================== \n");
    printf ("   Sección crítica: solución con semáforos. \n");
    printf (" ========================================== \n");
    printf ("   Para evitar que la condición de carrera  \n");
    printf ("   perjudique al código, se protejen las    \n");
    printf ("   secciones críticas mediante semáforos.   \n");
    printf ("                                            \n");
    printf ("   Cuando la variable dato no esté siendo   \n");
    printf ("   modificada, podría haber uno o varios    \n");
    printf ("   lectores leyéndola al mismo tiempo. Por  \n");
    printf ("   otra parte, cuando la variable esté      \n");
    printf ("   siendo modificada, solamente habrá un    \n");
    printf ("   escritor sobre ella, y ningún lector en  \n");
    printf ("   curso.                                   \n");
    printf (" ========================================== \n");
    printf ("   Teclear Ctrl + C para finalizar la       \n");
    printf ("   ejecución del programa.                  \n");
    printf (" ========================================== \n");
    printf (" Pulsar cualquier tecla para continuar... ");
    getchar ();

    printf ("\n");

    //----------------------------------------------------(-)

    // Inicialización semáforos
    sem_init (&mutex, 0, 1);
    sem_init (&m_escritores, 0, 1);
    
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
      pthread_create(&escritores[i], &atrib, escritor, &argEscritores[i]);
    }

    // Esperar a que los hilos terminen
    for (i = 0; i < MAX_L; i++) {
      pthread_join(lectores[i], NULL);
    }
    
    for (i = 0; i < MAX_E; i++) {
      pthread_join(escritores[i], NULL);
    }
    
    // Acaba el main
    printf ("Acaba el main\n");
    return 0;
}
