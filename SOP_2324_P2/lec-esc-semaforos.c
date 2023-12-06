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

#define MAX_L  3
#define MAX_E  2 


int dato = 0;

sem_t mutex;        // Semáforo para proteger las secciones críticas
sem_t m_escritores; // Semáforo para controlar el acceso de los escritores
int nlectores = 0;  // Variable que indica el número de lectores leyendo

void *lector(void *arg) {
    int id = *((int *)arg);
    while (1) {
       
        // 1) Incremento el número de lectores
        sem_wait(&mutex); // P(mutex)
        
        nlectores++;
        
        // 2) Si soy el primer lector entonces intento entrar a leer
        if(nlectores == 1) { sem_wait(&m_escritores); } // P(m_escritores)
        sem_post(&mutex); // V(mutex)
        
        usleep(1);
        
        // 3) Leer datos
        printf("Lector %d leyendo: %d\t\tnl: %d\n", id, dato, nlectores);
        
        // 4) Decremento el número de lectores
        sem_wait(&mutex); // P(mutex)
        nlectores--;
        
        // 5) Si soy el último, entonces desbloqueo al posible escritor que esté esperando
        if(nlectores == 0) { sem_post(&m_escritores); } // V(m_escritores)
        sem_post(&mutex); // V(mutex)
        
        // 6) Retraso aleatorio de hasta 1 segundo (en microsegundos)
        usleep(rand() % 1000000);
    }
}

void *escritor(void *arg) {
    int id = *((int *)arg);
    int aux;
    while (1) {
	
	// 1) Si no hay lectores entonces intento entrar a escribir
	sem_wait(&m_escritores); // P(m_escritores)
	        
    // 2) Escribir datos
    aux = dato;
    usleep(rand() % 1000000);
    aux++;
    usleep(rand() % 1000000);
    dato = aux;
    printf("Escritor %d escribiendo: %d\n", id, dato);
        
    // 3) Acabo de escribir, entonces desbloqueo al posible escritor que esté esperando
	sem_post(&m_escritores); // V(m_escritores)

	// 4) Retraso aleatorio de hasta 2 segundo (en microsegundos)
        usleep(rand() % 2000000);
    }
}

int main() {
    

    // Inicialización semáforos
    sem_init(&mutex, 0, 1);
    sem_init(&m_escritores, 0, 1);
    
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
