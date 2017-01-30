//error.h
//Riesenie IJC-DU1, priklad b),7.3.2015
//Autor: Matej Marusak, xmarus06, VUT FIT
//prelozene gcc 4.8.3
// rozhranie pre error.c pre chybove hlasanie

#include <stdarg.h>

#ifndef USE_ERRORS
#define USE_ERRORS

void Warning(const char *ftm, ...);

void FatalError(const char *ftm, ...);

#endif
