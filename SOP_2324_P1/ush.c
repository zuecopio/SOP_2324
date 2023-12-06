/*-----------------------------------------------------+
 |     U S H . C                                       |
 +-----------------------------------------------------+
 |     Versión     : microshell_v4                     | 
 |     Autor       : Marcos Belda Martínez             |
 |     Asignatura  : SOP-GIIROB                        |     
 |     Descripción : Programación de un "microshell"   |
 +-----------------------------------------------------*/

#include "defines.h"
#include "analizador.h"
#include "redireccion.h"
#include "ejecucion.h"
#include <string.h>
#include <stdio.h>



//-----[ DECLARACIONES DE FUNCIONES LOCALES ]--------------(+)

void visualizar( void );
int leerLinea( char *linea, int tamanyLinea );

//---------------------------------------------------------(-)




//-----[ PROGRAMA PRINCIPAL ]------------------------------(+)

int main(int argc, char * argv[])
{

  system ("clear"); // para tener una mejor inmersión en el Micro Shell
    
  char line[255];
  int res;
  char **m_ordenes;
  char ***m_argumentos;
  int *m_num_arg;
  int m_n;
  
  
  //·····[ DECLARACIÓN ÓRDENES INTERNAS ]············(+)
  
  char *internas[2] = { "cd", "exit" };
  int interna = -1;
  char *lastDir;
  int argExit;
  
  //·················································(-)


  while(1)
    {
    
      do {
	res = leerLinea(line, MAXLINE);
	  
	if (res == -2) {
	  fprintf(stdout, "logout\n\n");
	  exit(0);
	}
	if (res == -1)
	  fprintf(stdout, RED "ERROR: " RESET_COLOR "Linea muy larga\n");
	    
      } while (res < 0);

      if (analizar(line) == OK)
	{
	  m_n=num_ordenes();
	  m_num_arg=num_argumentos();
	  m_ordenes=get_ordenes();
	  m_argumentos=get_argumentos();
	  
	  
	  if (m_n > 0)
	    {
	      
	      //·····[ ¿ LINE == ORDEN INTERNA ? ]···············(+)
	  
	        for (int i = 0; i < 2; i++)
	          if (strcmp(m_ordenes[0], internas[i]) == 0) { interna = i; }

	      //·················································(-)
	      
	      
	      
	      //·····[ LINE == ORDEN INTERNA ] ··················(+)
	      
	      if (interna != -1) {
	        
	        //+++++[ CD ]++++++++++++++++++++++++++(+)
		
	        if (interna == 0) {

		  // CD
	          if ( m_num_arg[0] == 1 ) {
	            char *aux = NULL;
	            aux = getcwd (aux, MAXLINE);
	            lastDir = aux;
	            char *home = getenv("HOME");
	            chdir(home);
	          }

		  // CD -        
	          else if ( strcmp(m_argumentos[0][1], "-") == 0 ) {
	            char *aux = NULL;
	            aux = getcwd (aux, MAXLINE);
	            chdir(lastDir);
	            lastDir = aux;
	          }

		  // CD + DIRECTORIO	        
	          else {
	            char *aux = NULL;
	            aux = getcwd (aux, MAXLINE);
	            if (chdir(m_argumentos[0][1]) == -1)
	              fprintf(stdout, RED "ERROR: " RESET_COLOR "Directorio incorrecto\n");
	            else { lastDir = aux; }
	          }
		  
	          interna = -1;
	        }
		//+++++++++++++++++++++++++++++++++++++(-)

		

		//+++++[ EXIT ]++++++++++++++++++++++++(+)
	        
	        else {
	          if (m_num_arg[0] == 1) { exit(0); }
		  
	          else if (m_num_arg[0] == 2) {
	            if( argExit = (argExit = atoi(m_argumentos[0][1])) == 0) {
	              fprintf(stdout, RED "ERROR: " RESET_COLOR "Argumento invalido para la orden exit\n");
	              interna = -1;
	            }
	            else { exit(argExit); }
	          }
		  
		  else if (m_num_arg[0] > 2)
		    fprintf(stdout, RED "ERROR: " RESET_COLOR "Demasiados argumentos para la orden exit\n"); interna = -1;
	        }
		//+++++++++++++++++++++++++++++++++++++(-)
		
	      }
	      //·················································(-)


	      //·····[ LINE == ORDEN EXTERNA ] ··················(+)
	      
	      else      
	        if ( pipeline(m_n,fich_entrada(),fich_salida(),es_append(),es_background()) == OK )
		  ejecutar(m_n,m_num_arg,m_ordenes,m_argumentos,es_background());
	      
	      //·················································(-)
	    }

	  visualizar();
	  
	}
    }    

  return 0;
} // Fin de "main"

//---------------------------------------------------------(-)




/*************************************************************************/
/*                             leerLinea                                 */
/*-----------------------------------------------------------------------*/
/*                                                                       */
/* DESCRIPCIÓN:                                                          */
/* Obtiene la línea de órdenes para el mShell.                           */
/* Util para depuracion.                                                 */          
/*                                                                       */
/* ENTRADA:                                                              */
/* linea - puntero a un vector de carácteres donde se almancenan los     */
/*         caracteres leídos del teclado                                 */
/* tamanyLinea - tamaño máximo del vector anterior                       */
/* SALIDA:                                                               */
/* -- linea - si termina bien, contiene como último carácter el retorno  */
/*            de carro.                                                  */
/* -- leerLinea - Entero que representa el motivo de finalización de la  */
/*                función:                                               */
/*                  > 0 - terminación correcta, número de caracteres     */
/*                        leídos, incluído '\n'                          */
/*                   -1 - termina por haber alcanzado el número máximo de*/
/*                        caracteres que se espera leer de teclado, sin  */
/*                        encontrar '\n'.                                */
/*                   -2 - termina por haber leído fin de fichero (EOF)   */
/*                                                                       */
/*************************************************************************/

int leerLinea( char *linea, int tamanyLinea )
{
  char aux;
  int salida; // motivo de finalización de la función
  int i = 0; // recuento de carácteres para comprobar que no sobrepasa el límite de carácteres.
  char *direccion = NULL;

  direccion = getcwd (direccion, MAXLINE);

  printf (CYAN_T "%s" YELLOW PROMPT RESET_COLOR, direccion);
  
  do {
    aux = getchar();
    *linea = aux;
    linea++;
    i++;
  } while (aux != EOF && aux != '\n' && i < tamanyLinea);

  if (aux == EOF) { salida = -2; }
  
  else if (i >=  tamanyLinea) { salida = -1; }
  
  else { salida = i; }
  
  return salida;
  
} // Fin de "leerLinea"

/*************************************************************************/




/*************************************************************************/
/*                             visualizar                                */
/*-----------------------------------------------------------------------*/
/*                                                                       */
/* DESCRIPCIÓN:                                                          */
/* Visualiza los distintos argumentos de la orden analizada.             */
/* Util para depuracion.                                                 */
/*                                                                       */
/* ENTRADA: void                                                         */
/*                                                                       */
/* SALIDA: void                                                          */
/*                                                                       */
/*************************************************************************/

void visualizar( void )
{  
  
  int m_n = num_ordenes();
  char **m_ordenes = get_ordenes();
  int *m_num_arg = num_argumentos();
  char ***m_argumentos = get_argumentos();
  

  printf (WHITE);
  printf ("\n\n");
  printf ("*********************************************************\n");
  printf ("****    "                 LOGO                  "    ****\n");
  printf ("*********************************************************\n\n");
  printf ("** ORDENES\n");
  printf ("** Numero de ordenes: %d\n", m_n);
  printf ("** Orden en "); es_background() ? printf("background\n") : printf("foreground\n");

  
  for (int i = 0; i < m_n; i++) {
    printf ("** Orden %d\n", i);
    
    for (int j = 0; j < m_num_arg[i]; j++)
      printf ("**\tArgumento %d --> %s\n", j, m_argumentos[i][j]);
  }
  
  
  printf ("**********************************\n** REDIRECCION ENTRADA\n");
  
  if (*fich_entrada() != '\0')
    printf ("** Redireccion entrada a --> %s\n\n",fich_entrada());
  else
    printf ("** No hay redireccion de entrada\n\n");



  printf ("**********************************\n** REDIRECCION SALIDA\n");
  
  if (*fich_salida() != '\0') {
    
    printf ("** Redireccion salida a --> %s\n",fich_salida());
    
    if (es_append())
      printf ("** - Se ha escrito el nuevo contenido al final del fichero\n\n");
    else
      printf ("** - Se ha sobreescrito el contenido del fichero\n\n");
  }
  
  else
    printf("** No hay redireccion de salida\n\n");

  printf(RESET_COLOR);
   
} // Fin de "visualizar"

/*************************************************************************/
