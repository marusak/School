//error.c
//Riesenie IJC-DU1, priklad b),7.3.2015
//Autor: Matej Marusak, xmarus06, VUT FIT
//prelozene gcc 4.8.3
//modul pre chybove hlasanie

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "error.h"


void Warning(const char *ftm, ...){
    va_list ap;
    va_start(ap,ftm);
    fprintf(stderr,"CHYBA: ");
    vfprintf(stderr,ftm,ap);
    fprintf(stderr,"\n");
    va_end(ap);
}


void FatalError(const char *ftm, ...){

    va_list ap;
    va_start(ap,ftm);
    fprintf(stderr,"CHYBA: ");
    vfprintf(stderr,ftm,ap);
    fprintf(stderr,"\n");
    va_end(ap);
    exit(1);
}
