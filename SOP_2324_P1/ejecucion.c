/*-----------------------------------------------------+      
 |      E J E C U C I O N . C                          |
 +-----------------------------------------------------+
 |     Versión     : microshell_v4                     | 
 |     Autor       : Marcos Belda Martínez             |
 |     Asignatura  : SOP-GIIROB                        |     
 |     Descripción : Crea los procesos para ejecutar el|
 |                   argumento especificado en ordenes |
 +-----------------------------------------------------*/

#include "defines.h"
#include "redireccion.h"
#include "ejecucion.h"

// #include <signal.h> (añadido en defines.h)
// #include <sys/wait.h> (añadido en defines.h)





//-----[ EJECUTAR ]----------------------------------------(+)

int ejecutar (int nordenes,    // número de órdenes
	      int *nargs,      // número de argumentos por órden
	      char **ordenes,  // vector cadenas con las órdenes
	      char ***args,    // matriz con los argumentos
	      int bgnd)        // indica si la orden es en background
{

  int i, status, salidaBgnd = OK, salida = OK;
  pid_t aux[nordenes];
  pid_t auxBgnd[2];

  
  //·····[ EJECUCIÓN EN BACKGROUND ]·················(+)
  
  if (bgnd == 1) {
    
    auxBgnd[0] = fork();

    // CÓDIGO DEL HIJO (auxBgnd[0])
    if (auxBgnd[0] == 0) {

      auxBgnd[1] = fork();

      // CÓDIGO DEL HIJO (auxBgnd[1])
      //   - Continua a partir de la línea 68
      
      // CÓDIGO DEL PADRE (auxBgnd[1])
      if (auxBgnd[1] != 0) { cerrar_fd(); exit(1); }
    }
    

    // CÓDIGO DEL PADRE (ush)
    else {
      cerrar_fd();
      waitpid (auxBgnd[0], &status, 0) == auxBgnd[0];
      return salida;
    }
  }
  //·················································(-)

  

  //·····[ EJECUCIÓN FOREGROUND & (auxBgnd[1]) ]·····(+)
    
  for (i = 0; i < nordenes; i++) {
    
    aux[i] = fork();

    // CÓDIGO DEL HIJO (aux[i])
    if (aux[i] == 0) {

      redirigir_entrada(i);
      redirigir_salida(i);
      cerrar_fd();
      
      if (execvp( ordenes[i], args[i] ) == -1) {
	fprintf (stderr, RED "ERROR: " RESET_COLOR "Orden %d desconocida\n", i);
	exit(1);
      }
    }

    
    // CÓDIGO DEL PADRE (ush) o (auxBgnd[1])
    //   - Continua generando hijos mientras i < nordenes.
    
  }

  
  // CÓDIGO DEL PADRE (ush) o (auxBgnd[1])
  //   - Ya ha creado todo los hijos.

  cerrar_fd();

  for (int i = 0; i < nordenes; i++) {
    waitpid (aux[i], &status, 0) == aux[i];
    if (status == 256) { salida = ERROR; }
  }
     
  if (bgnd == 1) {    
    (salida == OK) ? (fprintf(stderr, "\nDone[%d]\n", getpid())) : (fprintf(stderr, "\nError[%d]\n", getpid()));
    exit(1);
    // EL SUBPROCESO (auxBgnd[1]) MUERE
    //   - Como su padre (auxBgnd[0]) ha acabado sin esperarle,
    //     a (auxBgnd[1]) le hace el wait el proceso con PID 1.  
  }
 
  else {
    return salida;
  }
  //·················································(-)
  
} // Fin de la funcion "ejecutar"

//---------------------------------------------------------(-)
