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
#define CYAN_T        "\e[1;36m"
#define YELLOW        "\e[1;33m"
#define WHITE         "\e[1;37m"
#define RED           "\e[0;31m"
  
//----------------------------------------------------(-)

#define MAX_L  3
#define MAX_E  2


int dato = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;         // Mutex para gestionar la exclusión mutua
pthread_cond_t cond_lectores = PTHREAD_COND_INITIALIZER;   // Variable condición para lectores
pthread_cond_t cond_escritores = PTHREAD_COND_INITIALIZER; // Variable condición para escritores
int nlectores = 0; // Variable que indica el número de lectores leyendo

void *lector(void *arg) {
    int id = *((int *)arg);
    while (1) {
       
        // 1) Cierro mutex
        pthread_mutex_lock(&mutex); // P(mutex)
        
            // 2) Incremento el número de lectores
            while (nlectores == -1) { pthread_cond_wait(&cond_lectores, &mutex); }
            nlectores++;
            //printf("nl: %d --> ", nlectores);
	
        // 3) Abro mutex
        pthread_mutex_unlock(&mutex); // V(mutex)

            // 4) Leer datos
            printf ( YELLOW " |  LECTORES  -> %3d | · |   leyendo    >>  %3d |" RESET_COLOR, id, dato);

        // 5) Cierro mutex
        pthread_mutex_lock(&mutex); // P(mutex)
        
            // 6) Decremento el número de lectores
            printf ( YELLOW " · |  nl -> %3d |\n" RESET_COLOR, nlectores);
            nlectores--;
            
            // 7) Si soy el último, entonces mando (una señal) al posible escritor que esté esperando
            if (nlectores == 0) { pthread_cond_broadcast(&cond_escritores); }
        
        // 8) Abro mutex
        pthread_mutex_unlock(&mutex); // V(mutex)
        
            // 9) Retraso aleatorio de hasta 1 segundo (en microsegundos)
            usleep(rand() % 1000000);
    }
}

void *escritor(void *arg) {
    int id = *((int *)arg);
    int aux;
    while (1) {
        
        // 1) Cierro el mutex
        pthread_mutex_lock(&mutex); // P(mutex)
        
            // 2) Si no se cumple la condición se suspende y abre el mutex
            while (nlectores != 0) { pthread_cond_wait(&cond_escritores, &mutex); }
            nlectores = -1;

        // 3) Abro el mutex
        pthread_mutex_unlock(&mutex); // V(mutex)

            // 4) Escribir datos
            aux = dato;
            usleep(rand() % 1000000);
            aux++;
            usleep(rand() % 1000000);
            dato = aux;
            printf ( CYAN_T " | ESCRITORES -> %3d | · | escribiendo  <<  %3d |\n" RESET_COLOR, id, dato);

        // 5) Cierro el mutex
        pthread_mutex_lock(&mutex); // P(mutex)

            // 6) Mando (una señal) al posible escritor que esté esperando
            nlectores = 0;
            pthread_cond_broadcast(&cond_lectores);

        // 7) Abro el mutex
        pthread_mutex_unlock(&mutex); // V(mutex)

            // 8) Retraso aleatorio de hasta 2 segundos (en microsegundos)
            usleep(rand() % 2000000);
    }
}

int main() {
    

    // MAX_L lectores y MAX_E escritores
    pthread_t lectores[MAX_L], escritores[MAX_E];
    int argLectores[MAX_L], argEscritores[MAX_E];
    int i;
    pthread_attr_t atrib;

    // Inicializar la semilla del generador de números aleatorios
    srand(time(NULL)); 

    // Inicializar la estructura pthread_attr
    pthread_attr_init(&atrib);

    // Crear lectores
    for(i = 0; i < MAX_L; i++) {
      argLectores[i] = i;
      pthread_create(&lectores[i], &atrib, lector, &argLectores[i]);
    }

    // Crear escritores
    for(i = 0; i < MAX_E; i++) {
      argEscritores[i] = i;
      pthread_create(&escritores[i], &atrib, escritor, &argEscritores[i]);
    }

    // Esperar a que los hilos terminen
    for(i = 0; i < MAX_L; i++) {
      pthread_join(lectores[i], NULL);
    }
    
    for(i = 0; i < MAX_E; i++) {
      pthread_join(escritores[i], NULL);
    }
    
    // Acaba el main
    printf("Acaba el main\n");
    return 0;
}
