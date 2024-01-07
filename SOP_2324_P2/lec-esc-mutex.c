/*-----------------------------------------------------+
 |     L E C - E S C - M U T E X . C                   |
 +-----------------------------------------------------+
 |     Versión     : LemonTree_v1                      | 
 |     Autor       : Marcos Belda Martínez             |
 |     Asignatura  : SOP-GIIROB                        |     
 |     Descripción : Sección crítica: solución con     |
 |                   mutexes y variables condición     |
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

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;          // Mutex para gestionar la exclusión mutua
pthread_cond_t cond_lectores = PTHREAD_COND_INITIALIZER;    // Variable condición para lectores
pthread_cond_t cond_escritores = PTHREAD_COND_INITIALIZER;  // Variable condición para escritores
int nlectores = 0;                                          // Variable que indica el número de lectores leyendo

void *lector (void *arg) {

    int id = *((int *)arg);
    while (1) {
       
        // 1) Cierro mutex
        pthread_mutex_lock (&mutex); // P(mutex)
        
            // 2) Si nlectores es -1, es que ya hay un escritor en marcha,
            //    entonces se suspende y abre el mutex. Si nlectores es 
            //    diferente de -1, incrementa el número de lectores. 
            while (nlectores == -1) { pthread_cond_wait(&cond_lectores, &mutex); }
            nlectores++;
	
        // 3) Abro mutex
        pthread_mutex_unlock (&mutex); // V(mutex)

            // 4) Leer datos
            // Como se ha implemntado un mútex para los escritores, cuando haya lectores leyendo, el nº de esc. concurrentes siempre será 0
            printf (LECTORES " |  LECTOR  ->  %d | · | dato >>  %3d | ······· " INFO_LEC "| lec. activos = %d | · | esc. activos = 0 |" RESET_COLOR "\n", id, dato, nlectores);

        // 5) Cierro mutex
        pthread_mutex_lock (&mutex); // P(mutex)
        
            // 6) Decremento el número de lectores
            nlectores--;
            
            // 7) Si soy el último, entonces aviso a los posibles escritores que estén esperando
            if (nlectores == 0) { pthread_cond_broadcast (&cond_escritores); }
        
        // 8) Abro mutex
        pthread_mutex_unlock (&mutex); // V(mutex)
        
            // 9) Retraso aleatorio de hasta 1 segundo (en microsegundos)
            usleep (rand() % 1000000);
    }
}

void *escritor (void *arg) {

    int id = *((int *)arg);
    int aux;
    while (1) {
        
        // 1) Cierro el mutex
        pthread_mutex_lock(&mutex); // P(mutex)
        
            // 2) Si nlectores es -1, es que ya hay un escritor en marcha, 
            //    y si nlectores > 0 es que hay lectores leyendo. Si se
            //    cumple la condición se suspende y abre el mutex. 
            while (nlectores != 0) { pthread_cond_wait (&cond_escritores, &mutex); }
            nlectores = -1;

        // 3) Abro el mutex
        pthread_mutex_unlock (&mutex); // V(mutex)

            // 4) Escribir datos
            aux = dato;
            usleep (rand() % 1000000);
            aux++;
            usleep (rand() % 1000000);
            dato = aux;
            // Cuando nlectores = -1, quiere decir que hay escritores, por lo tanto no hay lectores. El nº de esc. concurrentes siempre será 1 y nº de lec. será 0
            printf (ESCRITORES " | ESCRITOR ->  %d | · | dato <<  %3d |" RESET_COLOR LECTORES " ······· " INFO_ESC "| lec. activos = %d | · | esc. activos = 1 |" RESET_COLOR "\n", id, dato, nlectores+1);

        // 5) Cierro el mutex
        pthread_mutex_lock (&mutex); // P(mutex)

            // 6) Mando (una señal) al posible escritor que esté esperando
            nlectores = 0;
            pthread_cond_broadcast (&cond_lectores);

        // 7) Abro el mutex
        pthread_mutex_unlock (&mutex); // V(mutex)

            // 8) Retraso aleatorio de hasta 2 segundos (en microsegundos)
            usleep (rand() % 2000000);
    }
}

int main() {
    
    //-----[ MENÚ DE INICIO ]-----------------------------(+)
    
    printf ("\n");

    printf (" ============= LEC-ESC-MUTEX.C ============ \n");    
    printf (" ========================================== \n");
    printf ("   Sección crítica: solución con mutexes y  \n");
    printf ("   variables condición.                     \n");
    printf (" ========================================== \n");
    printf ("   Para evitar que la condición de carrera  \n");
    printf ("   perjudique al código, se protejen las    \n");
    printf ("   secciones críticas mediante mutexes y    \n");
    printf ("   variables condición.                     \n");
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
