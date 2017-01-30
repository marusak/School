//io.c
//Riesenie IJC-DU2, priklad b),10.4.2015
//Autor: Matej Marusak, xmarus06, VUT FIT
//prelozene gcc 4.8.3
//funkcia pre nacitanie slova

#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// vracia EOF pri EOF, 0 pri chybe, max pri skrateni slova, inak pocet precitanych znakov(maximalne max-1)
int fgetw(char *s, int max, FILE *f){
    if ( s == NULL || f == NULL) //kontrola platnosti ukazatelov
        return 0;

    if ( max <= 0)//kladne cislo
        return 0;

    int i = 0;
    int c;

    while ( (c=fgetc(f))!=EOF && isspace(c) != 0); // preskocim medzery na zaciatku
    
    if (c == EOF)
        return EOF;
    else *s = c;
    
    for (i = 1; i < max+1; i++){ //bolo len <max
        if ((c=fgetc(f)) == EOF)
           return EOF;
        if (isspace(c) != 0)
            break;
       *(++s) = c;
    }
    if (i != max+1){//menej ako ako sa max-1 bolo iba max
        *(++s) = '\0';
        return i;
    }
    else{
        if (*s != '\0'){
            *s = '\0';
            while ( (c=fgetc(f))!=EOF && isspace(c) == 0); // preskocim zvysok slova
            return max+1;//bolo bez +1
        }
        else return i;
    }
    return 0;
}
