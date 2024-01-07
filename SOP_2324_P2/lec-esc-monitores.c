/*-----------------------------------------------------+
 |     L E C - E S C - M O N I T O R E S . C           |
 +-----------------------------------------------------+
 |     Versión     : LemonTree_v1                      | 
 |     Autor       : Marcos Belda Martínez             |
 |     Asignatura  : SOP-GIIROB                        |     
 |     Descripción : Sección crítica: solución con     |
 |                   monitores (ampliación)            |
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

//-----[ DECLARACIÓN DE VARIABLES ]------------------------(+)

int dato = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;          // Mutex para gestionar la exclusión mutua
pthread_cond_t cond_lectores = PTHREAD_COND_INITIALIZER;    // Variable condición para lectores
pthread_cond_t cond_escritores = PTHREAD_COND_INITIALIZER;  // Variable condición para escritores

int nlec_esperando = 0;  // Variable que indica el número de lectores esperando
int nlec_activos = 0;    // Variable que indica el número de lectores leyendo
int nesc_esperando = 0;  // Variable que indica el número de escritores esperando
int nesc_activos = 0;    // Variable que indica el número de escritores activos

//---------------------------------------------------------(-)


//-----[ DECLARACIÓN DE FUNCIONES MONITOR ]----------------(+)

void comenzar_lectura () {
    
    // 1) Cierro mutex
    pthread_mutex_lock (&mutex); // P(mutex)

        // 2) Incremento el número de escritores esperando
        nlec_esperando++;

        // 3) Decremento nlec_esperando y aumento nlec_activos si puedo leer, si no me espero
        while ((nesc_activos) > 0 || (nesc_esperando > 0)) {
            pthread_cond_wait (&cond_lectores, &mutex);
        }
        nlec_esperando--;
        nlec_activos++;

    // 4) Abro mutex
    pthread_mutex_unlock (&mutex); // V(mutex)
}

void terminar_lectura () {
    
    // 1) Cierro mutex
    pthread_mutex_lock (&mutex); // P(mutex)

        // 2) Decremento el número de lectores
        nlec_activos--;
            
        // 3) Si soy el último lector, entonces mando (una señal) al posible escritor que esté esperando
        if (nlec_activos == 0) {
            pthread_cond_signal (&cond_escritores);
        }

    // 4) Abro mutex
    pthread_mutex_unlock (&mutex); // V(mutex)

        // 5) Retraso de 1 segundo
        sleep (1);
}

void comenzar_escritura () {
    
    // 1) Cierro mutex
    pthread_mutex_lock (&mutex); // P(mutex)

        // 2) Incremento el número de escritores esperando
        nesc_esperando++;

        // 3) Decremento nesc_esperando y aumento nesc_activos si puedo escribir, si no me espero
        if ((nlec_activos > 0) || (nesc_activos > 0)) {
            pthread_cond_wait (&cond_escritores, &mutex);
        }
        nesc_esperando--;
        nesc_activos++;

    // 4) Abro mutex
    pthread_mutex_unlock (&mutex); // V(mutex)
}

void terminar_escritura () {
    
    // 1) Cierro mutex
    pthread_mutex_lock (&mutex); // P(mutex)

        // 2) Decremento el número de escritores escribiendo
        nesc_activos--;

        // 3) Si soy el último escritor, mando (señales) a los posibles lectores que estén esperando
        if (nesc_esperando == 0) {
            pthread_cond_broadcast (&cond_lectores);
        }

        // 4) Si hay escritores esperando, mando (una señal) al posible escritor que esté esperando
        else {
            // pthread_cond_signal (&cond_escritores);
            pthread_cond_broadcast (&cond_escritores);
        }

    // 5) Abro mutex
    pthread_mutex_unlock (&mutex); // V(mutex)

        // 6) Retraso de 1 segundo
        sleep(1);
}

//---------------------------------------------------------(-)

void *lector (void *arg) {
    int id = *((int *)arg);
    while (1) {

        comenzar_lectura ();

            printf (LECTORES " |  LECTOR  ->  %d | · | dato >>  %3d | ······· " INFO_LEC "| lec. activos = %d | · | esc. activos = %d |" RESET_COLOR LECTORES" ··· " INFO_LEC "| lec. en espera = %d | · | esc. en espera = %d |" RESET_COLOR "\n", id, dato, nlec_activos, nesc_activos, nlec_esperando, nesc_esperando);

        terminar_lectura ();
    }
    pthread_exit (NULL);
}

void *escritor (void *arg) {
    int id = *((int *)arg); 
    int aux;
    while (1) {

        comenzar_escritura ();

            aux = dato;
            usleep (rand() % 1000000);
            aux++;
            usleep (rand() % 1000000);
            dato = aux;
            printf (ESCRITORES " | ESCRITOR ->  %d | · | dato >>  %3d |" RESET_COLOR LECTORES " ······· " INFO_ESC "| lec. activos = %d | · | esc. activos = %d |" RESET_COLOR LECTORES" ··· " INFO_ESC "| lec. en espera = %d | · | esc. en espera = %d |" RESET_COLOR "\n", id, dato, nlec_activos, nesc_activos, nlec_esperando, nesc_esperando);

        terminar_escritura ();
    }
    pthread_exit (NULL);
}

int main () {
    
    //-----[ MENÚ DE INICIO ]-----------------------------(+)
    
    printf ("\n");

    printf (" =========== LEC-ESC-MONITORES.C ========== \n");    
    printf (" ========================================== \n");
    printf ("   Sección crítica: solución con monitores  \n");
    printf (" ========================================== \n");
    printf ("   Para evitar que la condición de carrera  \n");
    printf ("   perjudique al código, se protejen las    \n");
    printf ("   secciones críticas mediante monitores.   \n");
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
