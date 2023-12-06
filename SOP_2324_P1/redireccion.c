/*-----------------------------------------------------+
 |     R E D I R E C C I O N . C                       |
 +-----------------------------------------------------+
 |     Versi�n     : microshell_v4                     |
 |     Autor       : Marcos Belda Mart�nez             |
 |     Asignatura  : SOP-GIIROB                        |
 |     Descripci�n : Funciones encargadas del manejo de|
 |                   descriptores de fichero necesarios|
 |                   para las utilizaci�n de tuber�as y|
 |                   redirecciones                     |
 +-----------------------------------------------------*/

#include "defines.h"
#include "analizador.h"
#include "redireccion.h"
#include <unistd.h>

#define NEWFILE (O_WRONLY | O_CREAT | O_TRUNC)
#define APPENDFILE (O_WRONLY | O_CREAT | O_APPEND)
#define MODE644 (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

CMDFD cmdfd;


/*-----------------------------------------------------+
 |     FASE 1                                          |
 +-----------------------------------------------------*/



//-----[ REDIRECCION INI ]---------------------------------(+)

void redireccion_ini(void)
{
  for ( int i = 0; i < PIPELINE-2; i++ ) {
    cmdfd[i].infd = 0;
    cmdfd[i].outfd = 1;
  }
} //Inicializar los valores de la estructura cmdfd

//---------------------------------------------------------(-)




//-----[ PIPELINE ]----------------------------------------(+)

int pipeline(int ncmd, char * infile, char * outfile, int append, int bgnd)
{
  redireccion_ini(); // lo primero

  int salida = OK;
  int fds[2];
  int fd;

  for ( int i = 0; i < (ncmd-1); i++ ) {
    pipe(fds);
    cmdfd[i].outfd = fds[1];
    cmdfd[i+1].infd = fds[0];
  }
  
  //�����[ REDIRECCI�N ENTRADA ]���������������������(+)
   
  if ( strcmp( infile, "" ) ) { // Si son iguales devuelve 0, no entra en el if()
    if ( (fd = open(infile, O_RDONLY)) == -1 ) {
      fprintf(stderr, RED "ERROR: " RESET_COLOR "Fallo al leer fichero de entrada \"%s\"\n", infile);
      salida = FALSE;
    }
    else { cmdfd[0].infd = fd; }
  }
  //�������������������������������������������������(-)


  //�����[ REDIRECCI�N SALIDA ]����������������������(+)
  
  if ( strcmp( outfile, "" ) ) { // Si son iguales devuelve 0, no entra en el if()
    if (append) { // la salida no se trunca
      if ( (fd = open(outfile, APPENDFILE, MODE644)) == -1 ) {
	fprintf(stderr, RED "ERROR: " RESET_COLOR "Fallo al leer fichero de salida \"%s\"\n", outfile);
	salida = FALSE;
      }
    }
    else { // la salida es truncada
      if ( (fd = open(outfile, NEWFILE, MODE644)) == -1 ) {
	fprintf (stderr, RED "ERROR: " RESET_COLOR "Fallo al leer fichero de salida \"%s\"\n", outfile);
	salida = FALSE;
      }
    }
    cmdfd[ncmd-1].outfd = fd;
  }
  
  
  //+++++[ SALIDA A "/dev/null" ]++++++++(+)
  //
  //   - Cuando se ejecuta en background, si anteriormente no ha habido una redirecci�n
  //     de salida a cualquier fichero, la orden redirige su SALIDA estandar a "/dev/null". De esta forma
  //     solo veremos un mensaje informativo referente a la finalizaci�n de la ejecuci�n (Done o Exit),
  //     pero no veremos el resultado de la ejecuci�n en pantalla.
  //
  //   - Si se quisiera ver el resultado de la ejecuci�n es tan sencillo como hacer una redirecci�n de
  //     salida a un fichero y as� el resultado no ir� a parar a "/dev/null"
  //
  //   - Al redirigir la entrada de un proceso desde "/dev/null/", con la ejecuci�n de ciertas �rdenes no
  //     notar�amos ninguna diferencia, pero en algunas �rdenes como por ejemplo "ls", mostrar�a un mensaje
  //     vac�o, ya que en "/dev/null/" no hay nada que mostrar como lista.
  
  else {
    if ( bgnd == 1 ) {
      if ( (fd = open("/dev/null", NEWFILE, MODE644)) == -1 ) {
        fprintf(stderr, RED "ERROR: " RESET_COLOR "Fallo al leer fichero de salida \"/dev/null\"\n");
        salida = FALSE;
      }
      else { cmdfd[ncmd-1].outfd = fd; }
    }
  }
  //+++++++++++++++++++++++++++++++++++++(-)
  
  //�������������������������������������������������(-)
  
  
  return salida;
} // Fin de la funci�n "pipeline"

//---------------------------------------------------------(-)




/*-----------------------------------------------------+
 |     FASE 2                                          |
 +-----------------------------------------------------*/



//-----[ REDIRIGIR ENTRADA ]-------------------------------(+)

int redirigir_entrada(int i)
{
  int salida = OK;
  
  if (cmdfd[i].infd != 0) {
    if (dup2 (cmdfd[i].infd, 0) == -1) {
      fprintf (stderr, RED "ERROR: " RESET_COLOR "Fallo al redirigir entrada. Orden %d\n", i);
      salida = ERROR;
    }
  }
  
  return salida;
    
} // Fin de la funci�n "redirigir_entrada"

//---------------------------------------------------------(-)





//-----[ REDIRIGIR SALIDA ]--------------------------------(+)

int redirigir_salida(int i)
{
  int salida = OK;
  
  if (cmdfd[i].outfd != 1) {
    if (dup2 (cmdfd[i].outfd, 1) == -1) {
      fprintf (stderr, RED "ERROR: " RESET_COLOR "Fallo al redirigir salida. Orden %d\n", i);
      salida = ERROR;
    }
  }
  
  return salida;
    
} // Fin de la funci�n "redirigir_salida"

//---------------------------------------------------------(-)




/*-----------------------------------------------------+
 |     FASE 3                                          |
 +-----------------------------------------------------*/



//-----[ CERRAR FD ]---------------------------------------(+)

int cerrar_fd()
{  
  for ( int i = 0; i < PIPELINE-2; i++ ) {
    if ( cmdfd[i].infd != 0 ) {
      close(cmdfd[i].infd);
    }

    if ( cmdfd[i].outfd != 1 ) {
      close(cmdfd[i].outfd);
    }
  }
  
  return OK;
  
} // Fin de la funci�n "cerrar_fd"

//---------------------------------------------------------(-)
