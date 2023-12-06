/*-----------------------------------------------------+      
 |     E J E C U C I O N . H                           |
 +-----------------------------------------------------+
 |     Versión     : microshell_v4                     | 
 |     Autor       : Marcos Belda Martínez             |
 |     Asignatura  : SOP-GIIROB                        |     
 |     Descripción : Crea los procesos para ejecutar el|
 |                   argumento especificado en ordenes |
 +-----------------------------------------------------*/

#ifndef DEFINES_H
#define DEFINES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> // para ejecucion.c
#include <fcntl.h>
#include <errno.h>
#include <signal.h> // para ejecucion.c


//-----[ AÑADIDO POR EL ALUMNO ]----------------------(+)

  #include <sys/wait.h>
  #include <unistd.h>

  #define RESET_COLOR	"\e[0m"
  #define CYAN_T   	"\e[1;36m"
  #define YELLOW   	"\e[1;33m"
  #define WHITE         "\e[1;37m"
  #define RED           "\e[0;31m"
  
//----------------------------------------------------(-)


#define max(a,b) (((a)>(b)) ? (a) : (b))

#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0


//-----[ MODIFICADO POR EL ALUMNO ]-------------------(+)

  #define LOGO "MICRO SHELL (c)2023 Marcos Belda Martínez"
  
//----------------------------------------------------(-)


#define PROMPT " : ush_$ "

#define MAXLINE 200
#define MAXARG 20
#define MAXNAME 100
#define PIPELINE 10


typedef struct
{
  int infd;
  int outfd;
}CMDFD[PIPELINE-1];


#endif
