//htab_free.c
//Riesenie IJC-DU2, priklad b),10.4.2015
//Autor: Matej Marusak, xmarus06, VUT FIT
//prelozene gcc 4.8.3
//funkcia pre zrusenie tabulky

#include "htable.h"

void htab_free( htab_t *t){
    htab_clear(t);//uvolnim vsetky polozky
    free(t);//uvolnim tabulku
}
